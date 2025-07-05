import argparse
import os
import requests
import subprocess
from urllib.parse import urljoin, urlparse
from tqdm import tqdm
import tempfile
import shutil

def resolve_media_playlist(m3u8_url):
    """
    Recursively resolve m3u8 playlists until a media playlist (with .ts segments) is found.
    If a master playlist is found, select the highest bandwidth stream.
    Returns the lines of the media playlist and the final URL.
    """
    visited = set()
    while True:
        if m3u8_url in visited:
            raise RuntimeError(f"Circular reference detected in playlists: {m3u8_url}")
        visited.add(m3u8_url)
        r = requests.get(m3u8_url)
        r.raise_for_status()
        lines = r.text.strip().splitlines()
        # Check for .ts segments (allow query params)
        ts_urls = [line for line in lines if line and not line.startswith("#") and ".ts" in line]
        if ts_urls:
            return lines, m3u8_url
        # Check for master playlist with #EXT-X-STREAM-INF
        stream_infos = []
        for i, line in enumerate(lines):
            if line.startswith("#EXT-X-STREAM-INF:"):
                # Parse bandwidth
                attrs = line[len("#EXT-X-STREAM-INF:"):]
                bandwidth = 0
                for attr in attrs.split(','):
                    if attr.strip().startswith("BANDWIDTH="):
                        try:
                            bandwidth = int(attr.strip().split('=')[1])
                        except Exception:
                            pass
                # The next non-empty, non-comment line is the playlist URL
                for j in range(i+1, len(lines)):
                    next_line = lines[j].strip()
                    if next_line and not next_line.startswith("#") and ".m3u8" in next_line:
                        stream_infos.append((bandwidth, urljoin(m3u8_url, next_line)))
                        break
        if stream_infos:
            # Pick the highest bandwidth
            stream_infos.sort(reverse=True)
            next_m3u8_url = stream_infos[0][1]
            print(f"Following variant stream: {next_m3u8_url}")
            m3u8_url = next_m3u8_url
            continue
        # Fallback: follow any .m3u8 line
        for line in lines:
            line = line.strip()
            if line and not line.startswith("#") and ".m3u8" in line:
                m3u8_url = urljoin(m3u8_url, line)
                break
        else:
            raise RuntimeError(f"No .ts segments or nested .m3u8 found in playlist: {m3u8_url}")


def download_m3u8(m3u8_url, output_filename="output.mp4"):
    temp_dir = tempfile.mkdtemp(prefix="hls_dl_")

    try:
        # Step 1: Download .m3u8 playlist (recursively resolve if needed)
        print("Resolving playlist...")
        lines, final_m3u8_url = resolve_media_playlist(m3u8_url)

        # Step 2: Extract .ts segment URLs (allow query params)
        ts_urls = [urljoin(final_m3u8_url, line) for line in lines if line and not line.startswith("#") and ".ts" in line]
        print(f"Found {len(ts_urls)} segments.")

        # Step 3: Download .ts files
        ts_paths = []
        for i, ts_url in enumerate(tqdm(ts_urls, desc="Downloading segments")):
            ts_path = os.path.join(temp_dir, f"seg_{i:05}.ts")
            with requests.get(ts_url, stream=True) as seg_resp:
                seg_resp.raise_for_status()
                with open(ts_path, 'wb') as f:
                    for chunk in seg_resp.iter_content(1024 * 1024):
                        f.write(chunk)
            ts_paths.append(ts_path)

        # Step 4: Merge segments into a single .ts file
        merged_path = os.path.join(temp_dir, "merged.ts")
        with open(merged_path, 'wb') as merged:
            for path in ts_paths:
                with open(path, 'rb') as seg:
                    shutil.copyfileobj(seg, merged)

        # Step 5: Convert to mp4 using ffmpeg
        print("Converting to mp4 with ffmpeg...")
        subprocess.run([
            "ffmpeg", "-y", "-i", merged_path,
            "-c", "copy", output_filename
        ], check=True)

        print("Finished")

    finally:
        shutil.rmtree(temp_dir)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Download and convert HLS video from .m3u8 URL")
    parser.add_argument("m3u8_url", help="Full URL to the .m3u8 playlist")
    parser.add_argument("-o", "--output", help="Output MP4 file name", default="output.mp4")
    args = parser.parse_args()

    download_m3u8(args.m3u8_url, args.output)