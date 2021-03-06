/*
 Copyright (c) 2014, Robot Control and Pattern Recognition Group, Warsaw University of Technology
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the Warsaw University of Technology nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL <COPYright HOLDER> BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "barrett_hand_gazebo.h"
#include <rtt/Component.hpp>

    BarrettHandGazebo::BarrettHandGazebo(std::string const& name)
        : TaskContext(name, RTT::TaskContext::PreOperational)
        , too_big_force_counter_(3, 0)
        , data_valid_(false)
//        , port_q_out_("q_OUTPORT", false)
//        , port_t_out_("t_OUTPORT", false)
//        , port_status_out_("status_OUTPORT", false)
        , disable_component_(false)
        , can_id_base_(-1)
        , first_step_(true)
        , sp_kp_(80)
        , sp_ki_(20)
        , sp_kd_(0)
        , sp_min_i_(-10)
        , sp_max_i_(10)
        , sp_min_cmd_(-80)
        , sp_max_cmd_(80)
        , k2_kp_(40)
        , k2_ki_(10)
        , k2_kd_(0)
        , k2_min_i_(-5)
        , k2_max_i_(5)
        , k2_min_cmd_(-40)
        , k2_max_cmd_(40)
        , k3_kp_(20)
        , k3_ki_(5)
        , k3_kd_(0)
        , k3_min_i_(-2.5)
        , k3_max_i_(2.5)
        , k3_min_cmd_(-20)
        , k3_max_cmd_(20)
    {
        addProperty("prefix", prefix_);
        addProperty("disable_component", disable_component_);
        addProperty("can_id_base", can_id_base_);

        addProperty("sp_kp", sp_kp_);
        addProperty("sp_ki", sp_ki_);
        addProperty("sp_kd", sp_kd_);
        addProperty("sp_min_i", sp_min_i_);
        addProperty("sp_max_i", sp_max_i_);
        addProperty("sp_min_cmd", sp_min_cmd_);
        addProperty("sp_max_cmd", sp_max_cmd_);
        addProperty("k2_kp", k2_kp_);
        addProperty("k2_ki", k2_ki_);
        addProperty("k2_kd", k2_kd_);
        addProperty("k2_min_i", k2_min_i_);
        addProperty("k2_max_i", k2_max_i_);
        addProperty("k2_min_cmd", k2_min_cmd_);
        addProperty("k2_max_cmd", k2_max_cmd_);
        addProperty("k3_kp", k3_kp_);
        addProperty("k3_ki", k3_ki_);
        addProperty("k3_kd", k3_kd_);
        addProperty("k3_min_i", k3_min_i_);
        addProperty("k3_max_i", k3_max_i_);
        addProperty("k3_min_cmd", k3_min_cmd_);
        addProperty("k3_max_cmd", k3_max_cmd_);

        // Add required gazebo interfaces
        this->provides("gazebo")->addOperation("configure",&BarrettHandGazebo::gazeboConfigureHook,this,RTT::ClientThread);
        this->provides("gazebo")->addOperation("update",&BarrettHandGazebo::gazeboUpdateHook,this,RTT::ClientThread);

        // right hand ports
//        this->ports()->addPort("q_INPORT",      port_q_in_);
//        this->ports()->addPort("v_INPORT",      port_v_in_);
//        this->ports()->addPort("t_INPORT",      port_t_in_);
//        this->ports()->addPort("mp_INPORT",     port_mp_in_);
//        this->ports()->addPort("hold_INPORT",   port_hold_in_);
//        this->ports()->addPort(port_q_out_);
//        this->ports()->addPort(port_t_out_);
//        this->ports()->addPort(port_status_out_);
        //this->ports()->addPort("BHTemp",        port_temp_out_);
//        this->ports()->addPort("max_measured_pressure_INPORT", port_max_measured_pressure_in_);
//        this->ports()->addPort("reset_fingers_INPORT", port_reset_in_);
//        q_in_.setZero();
//        v_in_.setZero();
//        t_in_.setZero();
        mp_in_ = 0.0;
        hold_in_ = 0;    // false
        max_measured_pressure_in_.setZero();
        //temp_out_.temp.resize(8);
        //port_temp_out_.setDataSample(temp_out_);
//        status_out_ = STATUS_IDLE1 | STATUS_IDLE2 | STATUS_IDLE3 | STATUS_IDLE4;
        for (int i = 0; i < 4; ++i) {
//            status_idle_[i] = true;
            status_overcurrent_[i] = false;
//            move_hand_[i] = false;
        }
        clutch_break_[0] = clutch_break_[1] = clutch_break_[2] = false;
    }

    BarrettHandGazebo::~BarrettHandGazebo() {
    }

ORO_LIST_COMPONENT_TYPE(BarrettHandGazebo)

