// Copyright (c) 2014, Solder Spot
// All rights reserved.
// See LICENSE.txt in root folder 

#include "Navigator.h"
#include <math.h>

//----------------------------------------
//
//----------------------------------------

Navigator::Navigator()
:m_dist_per_tick(0),
 m_encoder_heading_bias(0.0f),
 m_min_dt(nvMS(10))
{
	m_init_pose.position.x = 0.0f;
	m_init_pose.position.y = 0.0f;
	m_init_pose.heading = 0.0f;
    Reset( 0 );
}

//----------------------------------------
//
//----------------------------------------

void Navigator::Init( nvDistance wheel_diameter, nvDistance wheel_base, uint16_t ticks_per_rev )
{
    m_dist_per_tick = ticks_per_rev > 0 ? wheel_diameter*M_PI/ticks_per_rev : 0.0f;
    m_wheel_base = wheel_base > 1.0f ? wheel_base : 1.0f; 
}

//----------------------------------------
//
//----------------------------------------

void Navigator::Reset( nvTime now )
{
    m_last_ticks_time = now;
	m_dt = m_lticks = m_rticks = 0.0f;
	m_pose = m_init_pose;
	m_heading = nvDegToRad(m_pose.heading);
    m_speed = nvMM(0.0);
    m_turn_rate = nvDEGREES(0.0);
}

//----------------------------------------
//
//----------------------------------------

bool Navigator::UpdateTicks( int16_t lticks, int16_t rticks, nvTime now )
{
	// update delta values
	m_dt +=  nvDeltaTime( m_last_ticks_time, now );
	m_lticks += lticks;
	m_rticks += rticks;

	// remember time for next call
	m_last_ticks_time = now;

	// see if we have accumulated min time delta 
	if ( m_dt < m_min_dt )
	{
		// no, so wait
		return false;
	}

	// use ticks and time delta to update position

	nvDistance dr = ((nvDistance)m_rticks)*m_dist_per_tick;
	nvDistance dl = ((nvDistance)m_lticks)*m_dist_per_tick;
	nvDistance dd =  (dr + dl)/2;

    // calc and update change in heading
    nvRadians dh = (dl - dr)/m_wheel_base;
	m_heading = nvClipRadians( m_heading + dh);

	// update velocities
	m_speed = (dd*1000.0f)/m_dt;
    m_turn_rate = (nvRadToDeg(dh)*1000.0f)/m_dt;
        
        // update pose
	m_pose.heading = nvRadToDeg(m_heading);
	m_pose.position.x += dd*sin(m_heading);
	m_pose.position.y += dd*cos(m_heading);

        // reset delta values
	m_dt = 0;
	m_lticks = 0;
	m_rticks = 0;

	return true;
}

//----------------------------------------
//
//----------------------------------------

nvPosition Navigator::NewPosition( nvDistance distance )
{
	nvPosition pos;

	pos.x = m_pose.position.x + distance*sin(m_heading);
	pos.y = m_pose.position.y + distance*cos(m_heading);

	return pos;
}
