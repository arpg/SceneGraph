/*********************************************************************
* Software Ligcense Agreement (BSD License)
*
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: John Hsu */
/* Modified by: James Pizzurro */

#include "joint.h"
//#include <ros/console.h>
#include <boost/lexical_cast.hpp>

#include <SimpleGui/Robot/Constants.h>

namespace RGUtils
{
    bool JointDynamics::initXml(TiXmlElement* config)
    {
        this->clear();

        // Get joint damping
        const char* damping_str = config->Attribute("damping");
        if (damping_str == NULL){
            if( URDF_DEBUG )
                printf( " URDF DEBUG: joint dynamics: no damping, defaults to 0\n");
            this->damping = 0;
        }
        else
        {
            try
            {
                this->damping = boost::lexical_cast<double>(damping_str);
            }
            catch (boost::bad_lexical_cast &e)
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : damping value (%s) is not a float\n",damping_str);
                return false;
            }
        }

        // Get joint friction
        const char* friction_str = config->Attribute("friction");
        if (friction_str == NULL){
            if( URDF_DEBUG )
                printf( " URDF DEBUG: joint dynamics: no friction, defaults to 0\n");
            this->friction = 0;
        }
        else
        {
            try
            {
                this->friction = boost::lexical_cast<double>(friction_str);
            }
            catch (boost::bad_lexical_cast &e)
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : friction value (%s) is not a float\n",friction_str);
                return false;
            }
        }

        if (damping_str == NULL && friction_str == NULL)
        {
            if( URDF_DEBUG )
                printf( " URDF ERROR : joint dynamics element specified with no damping and no friction\n");
            return false;
        }
        else{
            if( URDF_DEBUG )
                printf( " URDF DEBUG: joint dynamics: damping %f and friction %f\n", damping, friction);
            return true;
        }
    }

    bool JointLimits::initXml(TiXmlElement* config)
    {
        this->clear();

        // Get lower joint limit
        const char* lower_str = config->Attribute("lower");
        if (lower_str == NULL){
            if( URDF_DEBUG )
                printf( " URDF DEBUG: joint limit: no lower, defaults to 0\n");
            this->lower = 0;
        }
        else
        {
            try
            {
                this->lower = boost::lexical_cast<double>(lower_str);
            }
            catch (boost::bad_lexical_cast &e)
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : lower value (%s) is not a float\n",lower_str);
                return false;
            }
        }

        // Get upper joint limit
        const char* upper_str = config->Attribute("upper");
        if (upper_str == NULL){
            if( URDF_DEBUG )
                printf( " URDF DEBUG: joint limit: no upper, , defaults to 0\n");
            this->upper = 0;
        }
        else
        {
            try
            {
                this->upper = boost::lexical_cast<double>(upper_str);
            }
            catch (boost::bad_lexical_cast &e)
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : upper value (%s) is not a float\n",upper_str);
                return false;
            }
        }

        // Get joint effort limit
        const char* effort_str = config->Attribute("effort");
        if (effort_str == NULL){
            if( URDF_DEBUG )
                printf( " URDF ERROR : joint limit: no effort\n");
            return false;
        }
        else
        {
            try
            {
                this->effort = boost::lexical_cast<double>(effort_str);
            }
            catch (boost::bad_lexical_cast &e)
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : effort value (%s) is not a float\n",effort_str);
                return false;
            }
        }

        // Get joint velocity limit
        const char* velocity_str = config->Attribute("velocity");
        if (velocity_str == NULL){
            if( URDF_DEBUG )
                printf( " URDF ERROR : joint limit: no velocity\n");
            return false;
        }
        else
        {
            try
            {
                this->velocity = boost::lexical_cast<double>(velocity_str);
            }
            catch (boost::bad_lexical_cast &e)
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : velocity value (%s) is not a float\n",velocity_str);
                return false;
            }
        }

        return true;
    }

    bool JointSafety::initXml(TiXmlElement* config)
    {
        this->clear();

        // Get soft_lower_limit joint limit
        const char* soft_lower_limit_str = config->Attribute("soft_lower_limit");
        if (soft_lower_limit_str == NULL)
        {
            if( URDF_DEBUG )
                printf( " URDF DEBUG: joint safety: no soft_lower_limit, using default value\n");
            this->soft_lower_limit = 0;
        }
        else
        {
            try
            {
                this->soft_lower_limit = boost::lexical_cast<double>(soft_lower_limit_str);
            }
            catch (boost::bad_lexical_cast &e)
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : soft_lower_limit value (%s) is not a float\n",soft_lower_limit_str);
                return false;
            }
        }

        // Get soft_upper_limit joint limit
        const char* soft_upper_limit_str = config->Attribute("soft_upper_limit");
        if (soft_upper_limit_str == NULL)
        {
            if( URDF_DEBUG )
                printf( " URDF DEBUG: joint safety: no soft_upper_limit, using default value\n");
            this->soft_upper_limit = 0;
        }
        else
        {
            try
            {
                this->soft_upper_limit = boost::lexical_cast<double>(soft_upper_limit_str);
            }
            catch (boost::bad_lexical_cast &e)
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : soft_upper_limit value (%s) is not a float\n",soft_upper_limit_str);
                return false;
            }
        }

        // Get k_position_ safety "position" gain - not exactly position gain
        const char* k_position_str = config->Attribute("k_position");
        if (k_position_str == NULL)
        {
            if( URDF_DEBUG )
                printf( " URDF DEBUG: joint safety: no k_position, using default value\n");
            this->k_position = 0;
        }
        else
        {
            try
            {
                this->k_position = boost::lexical_cast<double>(k_position_str);
            }
            catch (boost::bad_lexical_cast &e)
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : k_position value (%s) is not a float\n",k_position_str);
                return false;
            }
        }
        // Get k_velocity_ safety velocity gain
        const char* k_velocity_str = config->Attribute("k_velocity");
        if (k_velocity_str == NULL)
        {
            if( URDF_DEBUG )
                printf( " URDF ERROR : joint safety: no k_velocity\n");
            return false;
        }
        else
        {
            try
            {
                this->k_velocity = boost::lexical_cast<double>(k_velocity_str);
            }
            catch (boost::bad_lexical_cast &e)
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : k_velocity value (%s) is not a float\n",k_velocity_str);
                return false;
            }
        }

        return true;
    }

    bool JointCalibration::initXml(TiXmlElement* config)
    {
        this->clear();

        // Get rising edge position
        const char* rising_position_str = config->Attribute("rising");
        if (rising_position_str == NULL)
        {
            if( URDF_DEBUG )
                printf( " URDF DEBUG: joint calibration: no rising, using default value\n");
            this->rising.reset();
        }
        else
        {
            try
            {
                this->rising.reset(new double(boost::lexical_cast<double>(rising_position_str)));
            }
            catch (boost::bad_lexical_cast &e)
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : risingvalue (%s) is not a float\n",rising_position_str);
                return false;
            }
        }

        // Get falling edge position
        const char* falling_position_str = config->Attribute("falling");
        if (falling_position_str == NULL)
        {
            if( URDF_DEBUG )
                printf( " URDF DEBUG: joint calibration: no falling, using default value\n");
            this->falling.reset();
        }
        else
        {
            try
            {
                this->falling.reset(new double(boost::lexical_cast<double>(falling_position_str)));
            }
            catch (boost::bad_lexical_cast &e)
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : fallingvalue (%s) is not a float\n",falling_position_str);
                return false;
            }
        }

        return true;
    }

    bool JointMimic::initXml(TiXmlElement* config)
    {
        this->clear();

        // Get name of joint to mimic
        const char* joint_name_str = config->Attribute("joint");

        if (joint_name_str == NULL)
        {
            if( URDF_DEBUG )
                printf( " URDF ERROR : joint mimic: no mimic joint specified\n");
            //return false;
        }
        else
            this->joint_name = joint_name_str;

        // Get mimic multiplier
        const char* multiplier_str = config->Attribute("multiplier");

        if (multiplier_str == NULL)
        {
            if( URDF_DEBUG )
                printf( " URDF DEBUG: joint mimic: no multiplier, using default value of 1\n");
            this->multiplier = 1;
        }
        else
        {
            try
            {
                this->multiplier = boost::lexical_cast<double>(multiplier_str);
            }
            catch (boost::bad_lexical_cast &e)
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : multiplier value (%s) is not a float\n",multiplier_str);
                return false;
            }
        }


        // Get mimic offset
        const char* offset_str = config->Attribute("offset");
        if (offset_str == NULL)
        {
            if( URDF_DEBUG )
                printf( " URDF DEBUG: joint mimic: no offset, using default value of 0\n");
            this->offset = 0;
        }
        else
        {
            try
            {
                this->offset = boost::lexical_cast<double>(offset_str);
            }
            catch (boost::bad_lexical_cast &e)
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : offset value (%s) is not a float\n",offset_str);
                return false;
            }
        }

        return true;
    }

    bool Joint::initXml(TiXmlElement* config)
    {
        this->clear();

        // Get Joint Name
        const char *name = config->Attribute("name");
        if (!name)
        {
            if( URDF_DEBUG )
                printf( " URDF ERROR : unnamed joint found\n");
            return false;
        }
        this->name = name;

        // Get transform from Parent Link to Joint Frame
        TiXmlElement *origin_xml = config->FirstChildElement("origin");
        if (!origin_xml)
        {
            if( URDF_DEBUG )
                printf( " URDF DEBUG: Joint '%s' missing origin tag under parent describing transform from Parent Link to Joint Frame, (using Identity transform).\n", this->name.c_str());
            this->parent_to_joint_origin_transform.clear();
        }
        else
        {
            if (!this->parent_to_joint_origin_transform.initXml(origin_xml))
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : Malformed parent origin element for joint '%s'\n", this->name.c_str());
                this->parent_to_joint_origin_transform.clear();
                return false;
            }
        }

        // Get Parent Link
        TiXmlElement *parent_xml = config->FirstChildElement("parent");
        if (parent_xml)
        {
            const char *pname = parent_xml->Attribute("link");
            if (!pname)
            {
                if( URDF_DEBUG )
                    printf( " URDF INFO: no parent link name specified for Joint link '%s'. this might be the root?\n", this->name.c_str());
            }
            else
            {
                this->parent_link_name = std::string(pname);

            }
        }

        // Get Child Link
        TiXmlElement *child_xml = config->FirstChildElement("child");
        if (child_xml)
        {
            const char *pname = child_xml->Attribute("link");
            if (!pname)
            {
                if( URDF_DEBUG )
                    printf( " URDF INFO: no child link name specified for Joint link '%s'.\n", this->name.c_str());
            }
            else
            {
                this->child_link_name = std::string(pname);

            }
        }

        // Get Joint type
        const char* type_char = config->Attribute("type");
        if (!type_char)
        {
            if( URDF_DEBUG )
                printf( " URDF ERROR : joint '%s' has no type, check to see if it's a reference.\n", this->name.c_str());
            return false;
        }
        std::string type_str = type_char;
        if (type_str == "planar")
        {
            type = PLANAR;
            if( URDF_DEBUG )
                printf( " URDF WARN: Planar joints are deprecated in the URDF!\n");
        }
        else if (type_str == "floating")
        {
            type = FLOATING;
            if( URDF_DEBUG )
                printf( " URDF WARN: Floating joints are deprecated in the URDF!\n");
        }
        else if (type_str == "revolute")
            type = REVOLUTE;
        else if (type_str == "continuous")
            type = CONTINUOUS;
        else if (type_str == "prismatic")
            type = PRISMATIC;
        else if (type_str == "fixed")
            type = FIXED;
        else
        {
            if( URDF_DEBUG )
                printf( " URDF ERROR : Joint '%s' has no known type '%s'\n", this->name.c_str(), type_str.c_str());
            return false;
        }

        // Get Joint Axis
        if (this->type != FLOATING && this->type != FIXED)
        {
            // axis
            TiXmlElement *axis_xml = config->FirstChildElement("axis");
            if (!axis_xml){
                if( URDF_DEBUG )
                    printf( " URDF DEBUG: no axis elemement for Joint link '%s', defaulting to (1,0,0) axis\n", this->name.c_str());
                this->axis = Vector3(1.0, 0.0, 0.0);
            }
            else{
                if (!axis_xml->Attribute("xyz")){
                    if( URDF_DEBUG )
                        printf( " URDF ERROR : no xyz attribute for axis element for Joint link '%s'\n", this->name.c_str());
                }
                else {
                    if (!this->axis.init(axis_xml->Attribute("xyz")))
                    {
                        if( URDF_DEBUG )
                            printf( " URDF ERROR : Malformed axis element for joint '%s'\n", this->name.c_str());
                        this->axis.clear();
                        return false;
                    }
                }
            }
        }

        // Get limit
        TiXmlElement *limit_xml = config->FirstChildElement("limit");
        if (limit_xml)
        {
            limits.reset(new JointLimits);
            if (!limits->initXml(limit_xml))
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : Could not parse limit element for joint '%s'\n", this->name.c_str());
                limits.reset();
                return false;
            }
        }
        else if (this->type == REVOLUTE)
        {
            if( URDF_DEBUG )
                printf( " URDF ERROR : Joint '%s' is of type REVOLUTE but it does not specify limits\n", this->name.c_str());
            return false;
        }
        else if (this->type == PRISMATIC)
        {
            if( URDF_DEBUG )
                printf( " URDF INFO: Joint '%s' is of type PRISMATIC without limits\n", this->name.c_str());
            limits.reset();
        }

        // Get safety
        TiXmlElement *safety_xml = config->FirstChildElement("safety_controller");
        if (safety_xml)
        {
            safety.reset(new JointSafety);
            if (!safety->initXml(safety_xml))
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : Could not parse safety element for joint '%s'\n", this->name.c_str());
                safety.reset();
                return false;
            }
        }

        // Get calibration
        TiXmlElement *calibration_xml = config->FirstChildElement("calibration");
        if (calibration_xml)
        {
            calibration.reset(new JointCalibration);
            if (!calibration->initXml(calibration_xml))
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : Could not parse calibration element for joint  '%s'\n", this->name.c_str());
                calibration.reset();
                return false;
            }
        }

        // Get Joint Mimic
        TiXmlElement *mimic_xml = config->FirstChildElement("mimic");
        if (mimic_xml)
        {
            mimic.reset(new JointMimic);
            if (!mimic->initXml(mimic_xml))
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : Could not parse mimic element for joint  '%s'\n", this->name.c_str());
                mimic.reset();
                return false;
            }
        }

        // Get Dynamics
        TiXmlElement *prop_xml = config->FirstChildElement("dynamics");
        if (prop_xml)
        {
            dynamics.reset(new JointDynamics);
            if (!dynamics->initXml(prop_xml))
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : Could not parse joint_dynamics element for joint '%s'\n", this->name.c_str());
                dynamics.reset();
                return false;
            }
        }

        return true;
    }
}
