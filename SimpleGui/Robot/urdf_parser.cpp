/*********************************************************************
* Software License Agreement (BSD License)
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

/* Author: Wim Meeussen */
/* Modified by: James Pizzurro */

#include <boost/algorithm/string.hpp>
//#include <ros/console.h>
#include <vector>
#include "urdf_parser.h"

namespace RGUtils
{
    boost::shared_ptr<ModelInterface> parseURDF(const std::string &xml_string)
    {
        boost::shared_ptr<ModelInterface> model(new ModelInterface);
        model->clear();

        TiXmlDocument xml_doc;
        xml_doc.Parse(xml_string.c_str());

        TiXmlElement *robot_xml = xml_doc.FirstChildElement("robot");
        if (!robot_xml)
        {
            if( URDF_DEBUG )
                printf( " URDF ERROR : Could not find the 'robot' element in the xml file\n");
            model.reset();
            return model;
        }

        // Get robot name
        const char *name = robot_xml->Attribute("name");
        if (!name)
        {
            if( URDF_DEBUG )
                printf( " URDF ERROR : No name given for the robot.\n");
            model.reset();
            return model;
        }
        model->name_ = std::string(name);

        // Get all Material elements
        for (TiXmlElement* material_xml = robot_xml->FirstChildElement("material"); material_xml; material_xml = material_xml->NextSiblingElement("material"))
        {
            boost::shared_ptr<Material> material;
            material.reset(new Material);

            if (material->initXml(material_xml))
            {
                if (model->getMaterial(material->name))
                {
                    if( URDF_DEBUG )
                        printf( " URDF ERROR : material '%s' is not unique.\n", material->name.c_str());
                    material.reset();
                    model.reset();
                    return model;
                }
                else
                {
                    model->materials_.insert(make_pair(material->name,material));
                    if( URDF_DEBUG )
                        printf( " URDF DEBUG: successfully added a new material '%s'\n", material->name.c_str());
                }
            }
            else
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : material xml is not initialized correctly\n");
                material.reset();
                model.reset();
                return model;
            }
        }

        // Get all Link elements
        for (TiXmlElement* link_xml = robot_xml->FirstChildElement("link"); link_xml; link_xml = link_xml->NextSiblingElement("link"))
        {
            boost::shared_ptr<Link> link;
            link.reset(new Link);

            if (link->initXml(link_xml))
            {
                if (model->getLink(link->name))
                {
                    if( URDF_DEBUG )
                        printf( " URDF ERROR : link '%s' is not unique.\n", link->name.c_str());
                    model.reset();
                    return model;
                }
                else
                {
                    // set link visual material
                    if( URDF_DEBUG )
                        printf( " URDF DEBUG: setting link '%s' material\n", link->name.c_str());
                    if (link->visual)
                    {
                        if (!link->visual->material_name.empty())
                        {
                            if (model->getMaterial(link->visual->material_name))
                            {
                                if( URDF_DEBUG )
                                    printf( " URDF DEBUG: setting link '%s' material to '%s'\n", link->name.c_str(),link->visual->material_name.c_str());
                                link->visual->material = model->getMaterial( link->visual->material_name.c_str() );
                            }
                            else
                            {
                                if (link->visual->material)
                                {
                                    if( URDF_DEBUG )
                                        printf( " URDF DEBUG: link '%s' material '%s' defined in Visual.\n", link->name.c_str(),link->visual->material_name.c_str());
                                    model->materials_.insert(make_pair(link->visual->material->name,link->visual->material));
                                }
                                else
                                {
                                    if( URDF_DEBUG )
                                        printf( " URDF ERROR : link '%s' material '%s' undefined.\n", link->name.c_str(),link->visual->material_name.c_str());
                                    model.reset();
                                    return model;
                                }
                            }
                        }
                    }

                    model->links_.insert(make_pair(link->name,link));
                    if( URDF_DEBUG )
                        printf( " URDF DEBUG: successfully added a new link '%s'\n", link->name.c_str());
                }
            }
            else
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : link xml is not initialized correctly\n");
                model.reset();
                return model;
            }
        }
        if (model->links_.empty()){
            if( URDF_DEBUG )
                printf( " URDF ERROR : No link elements found in urdf file\n");
            model.reset();
            return model;
        }

        // Get all Joint elements
        for (TiXmlElement* joint_xml = robot_xml->FirstChildElement("joint"); joint_xml; joint_xml = joint_xml->NextSiblingElement("joint"))
        {
            boost::shared_ptr<Joint> joint;
            joint.reset(new Joint);

            if (joint->initXml(joint_xml))
            {
                if (model->getJoint(joint->name))
                {
                    if( URDF_DEBUG )
                        printf( " URDF ERROR : joint '%s' is not unique.\n", joint->name.c_str());
                    model.reset();
                    return model;
                }
                else
                {
                    model->joints_.insert(make_pair(joint->name,joint));
                    if( URDF_DEBUG )
                        printf( " URDF DEBUG: successfully added a new joint '%s'\n", joint->name.c_str());
                }
            }
            else
            {
                if( URDF_DEBUG )
                    printf( " URDF ERROR : joint xml is not initialized correctly\n");
                model.reset();
                return model;
            }
        }


        // every link has children links and joints, but no parents, so we create a
        // local convenience data structure for keeping child->parent relations
        std::map<std::string, std::string> parent_link_tree;
        parent_link_tree.clear();

        // building tree: name mapping
        if (!model->initTree(parent_link_tree))
        {
            if( URDF_DEBUG )
                printf( " URDF ERROR : failed to build tree\n");
            model.reset();
            return model;
        }

        // find the root link
        if (!model->initRoot(parent_link_tree))
        {
            if( URDF_DEBUG )
                printf( " URDF ERROR : failed to find root link\n");
            model.reset();
            return model;
        }

        return model;
    }
}
