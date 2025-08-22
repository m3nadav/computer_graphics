#ifndef COW_COORDINATES_H
#define COW_COORDINATES_H

// Cow body center coordinates (origin of cow's local coordinate system)
const float COW_BODY_CENTER_X = 0.0f;
const float COW_BODY_CENTER_Y = 0.0f;
const float COW_BODY_CENTER_Z = 0.0f;

// Head position relative to cow body center
const float COW_HEAD_X = 0.9f;
const float COW_HEAD_Y = 0.5f;
const float COW_HEAD_Z = 0.0f;

// Tail start position relative to cow body center
const float COW_TAIL_X = -0.866f;
const float COW_TAIL_Y = 0.15f;
const float COW_TAIL_Z = 0.0f;

// Leg positions relative to cow body center
const float COW_LEG_FRONT_LEFT_X = 0.4f;
const float COW_LEG_FRONT_LEFT_Y = -0.3f;
const float COW_LEG_FRONT_LEFT_Z = 0.25f;

const float COW_LEG_FRONT_RIGHT_X = 0.4f;
const float COW_LEG_FRONT_RIGHT_Y = -0.3f;
const float COW_LEG_FRONT_RIGHT_Z = -0.25f;

const float COW_LEG_BACK_LEFT_X = -0.4f;
const float COW_LEG_BACK_LEFT_Y = -0.3f;
const float COW_LEG_BACK_LEFT_Z = 0.25f;

const float COW_LEG_BACK_RIGHT_X = -0.4f;
const float COW_LEG_BACK_RIGHT_Y = -0.3f;
const float COW_LEG_BACK_RIGHT_Z = -0.25f;

#endif