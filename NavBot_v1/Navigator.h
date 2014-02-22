// Copyright (c) 2014, Solder Spot
// All rights reserved.
// See LICENSE.txt in root folder 

#ifndef __NAVIGATOR_H
#define __NAVIGATOR_H

#include <stdint.h>
#include <math.h>

//----------------------------------------
// Base Types
//----------------------------------------

typedef float       nvCoord;        // millimeters
typedef float       nvDegrees;      // degrees 
typedef float       nvRadians;      // radians 
typedef nvDegrees   nvHeading;      // degrees from North
typedef float       nvRate;         // change per second
typedef float       nvDistance;     // millimeters
typedef uint32_t    nvTime;         // time in milliseconds

// Time delta function that handles wrapround
#define nvMAX_TIME	0xffffffff
inline nvTime 		nvDeltaTime( nvTime last, nvTime now)	{ return now >= last ?  now - last : nvMAX_TIME - last + now + 1; }
// radians <-> degrees
inline nvDegrees 	nvRadToDeg( nvRadians rad ) { return (rad*180.0f)/M_PI; }
inline nvRadians	nvDegToRad( nvDegrees deg ) { return (deg*M_PI)/180.0f; }
inline nvDegrees	nvClipDegrees( nvDegrees deg ) { return deg - ( ((int32_t)deg/360L)*360.0f); }
inline nvRadians	nvClipRadians( nvRadians rad ) { return rad - ( ((int32_t)(rad/(2.0f*M_PI)))*2*M_PI); }

//----------------------------------------
// Helper Macros
//----------------------------------------

#define nvMM(D)         ((nvDistance)(D))
#define nvMETERS(D)     ((nvDistance)((D)*1000))
#define nvMS(MS)        ((nvTime)(MS))
#define nvSECONDS(S)    ((nvTime)((S)*1000))
#define nvDEGREES(D)    ((nvDegrees)(D))
#define nvRADIANSS(R)   ((nvRadians)(R))

#define nvNORTH         nvDEGREES(0)
#define nvNORTHEAST     nvDEGREES(45)
#define nvEAST          nvDEGREES(90)
#define nvSOUTHEAST     nvDEGREES(135)
#define nvSOUTH         nvDEGREES(180)
#define nvSOUTHWEST     nvDEGREES(225)
#define nvWEST          nvDEGREES(270)
#define nvNORTHWEST     nvDEGREES(315)

//----------------------------------------
// nvPosition
//----------------------------------------

struct nvPosition
{
    nvCoord     x;              // mm
    nvCoord     y;              // mm
};

//----------------------------------------
// nvPose
//----------------------------------------

struct nvPose
{
    nvPosition  position;       // mm from (0, 0)
    nvHeading   heading;        // degrees from North
};

//----------------------------------------
// Navigator
//----------------------------------------

class Navigator
{
    public:

        Navigator();

        // methods
        void            Init( nvDistance wheel_diameter, nvDistance wheel_base, uint16_t ticks_per_revolution );
        void            Reset( nvTime now );
        bool            UpdateTicks( int16_t lticks, int16_t rticks, nvTime now );

		// getters
        nvPose          Pose( void ) { return m_pose; }
        nvPosition      Position( void ) { return m_pose.position; }
        nvHeading       Heading( void ) { return m_pose.heading; }
        nvRate          Speed( void ) { return m_speed; }
        nvRate          TurnRate( void ) { return m_turn_rate; }
        float           TicksHeadingBias( void ) { return m_encoder_heading_bias; }

		// setters
		void            SetStartPose( const nvPose &pose) { m_init_pose.position = pose.position; m_init_pose.heading = nvClipDegrees( pose.heading); }
		void            SetStartPosition( const nvPosition &pos) { m_init_pose.position = pos; }
		void            SetStartPosition( nvCoord x, nvCoord y) { m_init_pose.position.x = x; m_init_pose.position.y = y; }
		void            SetStartHeading( nvHeading heading ) { m_init_pose.heading = nvClipDegrees(heading); }
		void            SetEncoderHeadingBias( float bias) { m_encoder_heading_bias = bias; }
		void			SetMinInterval( nvTime min ) { m_min_dt = min; }

		// helpers
		nvPosition		NewPosition( nvDistance distance );	

    protected:

		// spacial properties
        nvPose          m_pose;					// current pose
        nvRate          m_speed;                // current mm per second
        nvRate          m_turn_rate;            // current degrees per second

		// settings/config
        nvPose          m_init_pose;			// starting pose
        nvDistance      m_dist_per_tick;		// dist travelled per tick
        nvDistance      m_wheel_base;           // mm width of wheel base
        float           m_encoder_heading_bias;	// encoder bias compensation
		nvTime			m_min_dt;   			// minimum time delta unit

		// state data
        nvTime          m_last_ticks_time;		// time of last ticks update
		nvTime			m_dt;					// current time delta
		int16_t			m_lticks;				// current ticks
		int16_t			m_rticks;				// current ticks
		nvRadians		m_heading;				// current heading in radians

};


#endif // __NAVIGATOR_H