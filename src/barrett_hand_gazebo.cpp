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
#include <rtt/Logger.hpp>

using namespace RTT;

bool BarrettHandGazebo::gazeboConfigureHook(gazebo::physics::ModelPtr model) {
    Logger::In in("BarrettHandGazebo::gazeboConfigureHook");

    if(model.get() == NULL) {
        Logger::log() << Logger::Error <<  "the gazebo model is NULL" << Logger::endl;
        return false;
    }

    model_ = model;

    jc_ = new gazebo::physics::JointController(model_);

    return true;
}

double BarrettHandGazebo::clip(double n, double lower, double upper) const {
    return std::max(lower, std::min(n, upper));
}

double BarrettHandGazebo::getFingerAngle(unsigned int fidx) const {
    if (fidx < 3) {
        const int k2_jnt_tab[3] = {1, 4, 6};
        const int k3_jnt_tab[3] = {2, 5, 7};

        double k2_pos = joints_[k2_jnt_tab[fidx]]->Position();
        double k3_pos = joints_[k3_jnt_tab[fidx]]->Position();

        return k2_pos + (k3_pos - k2_pos / 3.0);
    } else if (fidx == 3) {
        int f1k1_jnt_idx = 0;
        int f2k1_jnt_idx = 3;
        return (joints_[f1k1_jnt_idx]->Position() + joints_[f2k1_jnt_idx]->Position()) / 2.0;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Update the controller
void BarrettHandGazebo::gazeboUpdateHook(gazebo::physics::ModelPtr model)
{
    if (disable_component_) {
        data_valid_ = true;
        return;
    }

    Logger::In in(std::string("BarrettHandGazebo::gazeboUpdateHook ") + getName());

    if (joints_.size() == 0) {
        return;
    }

    RTT::os::MutexTryLock trylock(gazebo_mutex_);
    if(!trylock.isSuccessful()) {
        return;
    }

    if (first_step_) {
        first_step_ = false;
        for (int i = 0; i < 8; i++) {
            jc_->AddJoint(joints_[i]);
        }

        double torque = 40.0;

/*
        jc_->SetPositionPID(joints_[0]->GetScopedName(), gazebo::common::PID(torque*2.0, torque*0.5, 0.0, torque*0.2, torque*(-0.2), torque*2.0,torque*(-2.0)));
        jc_->SetPositionPID(joints_[3]->GetScopedName(), gazebo::common::PID(torque*2.0, torque*0.5, 0.0, torque*0.2, torque*(-0.2), torque*2.0,torque*(-2.0)));
        jc_->SetPositionPID(joints_[1]->GetScopedName(), gazebo::common::PID(torque*1.1, torque*0.2, 0.0, torque*0.1, torque*(-0.1), torque*1.0,torque*(-1.0)));
        jc_->SetPositionPID(joints_[4]->GetScopedName(), gazebo::common::PID(torque*1.1, torque*0.2, 0.0, torque*0.1, torque*(-0.1), torque*1.0,torque*(-1.0)));
        jc_->SetPositionPID(joints_[6]->GetScopedName(), gazebo::common::PID(torque*1.1, torque*0.2, 0.0, torque*0.1, torque*(-0.1), torque*1.0,torque*(-1.0)));
        jc_->SetPositionPID(joints_[2]->GetScopedName(), gazebo::common::PID(torque*0.5, torque*0.1, 0.0, torque*0.04, torque*(-0.04), torque*0.7,torque*(-0.7)));
        jc_->SetPositionPID(joints_[5]->GetScopedName(), gazebo::common::PID(torque*0.5, torque*0.1, 0.0, torque*0.04, torque*(-0.04), torque*0.7,torque*(-0.7)));
        jc_->SetPositionPID(joints_[7]->GetScopedName(), gazebo::common::PID(torque*0.5, torque*0.1, 0.0, torque*0.04, torque*(-0.04), torque*0.7,torque*(-0.7)));
*/
        jc_->SetPositionPID(joints_[0]->GetScopedName(), gazebo::common::PID(torque*7.5, 0*0.1, 0.0, torque*0.2, torque*(-0.2), torque*8.0, torque*(-8.0)));
        jc_->SetPositionPID(joints_[3]->GetScopedName(), gazebo::common::PID(torque*7.5, 0*0.1, 0.0, torque*0.2, torque*(-0.2), torque*8.0, torque*(-8.0)));
        jc_->SetPositionPID(joints_[1]->GetScopedName(), gazebo::common::PID(torque*3.75, 0*0.05, 0.0, torque*0.1, torque*(-0.1), torque*4.0, torque*(-4.0)));
        jc_->SetPositionPID(joints_[4]->GetScopedName(), gazebo::common::PID(torque*3.75, 0*0.05, 0.0, torque*0.1, torque*(-0.1), torque*4.0, torque*(-4.0)));
        jc_->SetPositionPID(joints_[6]->GetScopedName(), gazebo::common::PID(torque*3.75, 0*0.05, 0.0, torque*0.1, torque*(-0.1), torque*4.0, torque*(-4.0)));
        jc_->SetPositionPID(joints_[2]->GetScopedName(), gazebo::common::PID(torque*2.5, 0*0.02, 0.0, torque*0.04, torque*(-0.04), torque*2.8, torque*(-2.8)));
        jc_->SetPositionPID(joints_[5]->GetScopedName(), gazebo::common::PID(torque*2.5, 0*0.02, 0.0, torque*0.04, torque*(-0.04), torque*2.8, torque*(-2.8)));
        jc_->SetPositionPID(joints_[7]->GetScopedName(), gazebo::common::PID(torque*2.5, 0*0.02, 0.0, torque*0.04, torque*(-0.04), torque*2.8, torque*(-2.8)));

        for (int i = 0; i < 8; i++) {
            jc_->SetPositionTarget(joints_[i]->GetScopedName(), joints_[i]->Position());
        }
    }

    // calculate sim period
    ros::Time now = rtt_rosclock::host_now();
    double vel_mult = std::max(1.0, (now - last_update_time_).toSec()/0.001);
    last_update_time_ = now;

    //
    // BarrettHand
    //

    const double force_factor = 1000.0;
    // joint position
    for (int i = 0; i < 8; i++) {
        q_out_(i) = joints_[i]->Position();
    }

    t_out_[0] = t_out_[3] = joints_[0]->GetForce(0)*force_factor;
    t_out_[1] = t_out_[2] = joints_[1]->GetForce(0)*force_factor;
    t_out_[4] = t_out_[5] = joints_[4]->GetForce(0)*force_factor;
    t_out_[6] = t_out_[7] = joints_[6]->GetForce(0)*force_factor;

    int f1k1_dof_idx = 3;
    int f1k1_jnt_idx = 0;
    int f2k1_dof_idx = 3;
    int f2k1_jnt_idx = 3;
    double mean_spread = getFingerAngle(3);

    for (int i = 0; i < 4; ++i) {
        if (hw_can_.move_hand_[i]) {
            hw_can_.move_hand_[i] = false;
            finger_int_[i] = getFingerAngle(i);
            hw_can_.status_idle_[i] = false;
            status_overcurrent_[i] = false;
            Logger::log() << Logger::Info <<  "move hand " << i << Logger::endl;
        }
    }

    if (!hw_can_.status_idle_[3]) {
        // spread joints
        if (finger_int_[3] > hw_can_.q_in_[f1k1_dof_idx]) {
            finger_int_[3] -= hw_can_.v_in_[f1k1_dof_idx] * vel_mult;
            if (finger_int_[3] <= hw_can_.q_in_[f1k1_dof_idx]) {
                hw_can_.status_idle_[3] = true;
                Logger::log() << Logger::Info <<  "spread idle" << Logger::endl;
            }
        }
        else if (finger_int_[3] < hw_can_.q_in_[f1k1_dof_idx]) {
            finger_int_[3] += hw_can_.v_in_[f1k1_dof_idx] * vel_mult;
            if (finger_int_[3] >= hw_can_.q_in_[f1k1_dof_idx]) {
                hw_can_.status_idle_[3] = true;
                Logger::log() << Logger::Info <<  "spread idle" << Logger::endl;
            }
        }

        double f1k1_force = joints_[f1k1_jnt_idx]->GetForce(0);
        double f2k1_force = joints_[f2k1_jnt_idx]->GetForce(0);
        double spread_force = f1k1_force + f2k1_force;

        if (!jc_->SetPositionTarget(joint_scoped_names_[f1k1_jnt_idx], finger_int_[3])) {
            Logger::log() << Logger::Warning <<  "jc_->SetPositionTarget(" << joint_scoped_names_[f1k1_jnt_idx] << ")" << Logger::endl;
        }
        if (!jc_->SetPositionTarget(joint_scoped_names_[f2k1_jnt_idx], finger_int_[3])) {
            Logger::log() << Logger::Warning <<  "jc_->SetPositionTarget(" << joint_scoped_names_[f2k1_jnt_idx] << ")" << Logger::endl;
        }
    }

    // finger joints
    const int k2_dof_tab[3] = {0, 1, 2};
    const int k2_jnt_tab[3] = {1, 4, 6};
    const int k3_jnt_tab[3] = {2, 5, 7};
    for (int fidx = 0; fidx < 3; fidx++) {
        int k2_dof = k2_dof_tab[fidx];
        int k2_jnt = k2_jnt_tab[fidx];
        int k3_jnt = k3_jnt_tab[fidx];
        bool is_opening = false;
//        double k2_force = joints_[k2_jnt]->GetForce(0);
//        double k3_force = joints_[k3_jnt]->GetForce(0);
        gazebo::physics::JointWrench k2_wrench = joints_[k2_jnt]->GetForceTorque(0);
        gazebo::physics::JointWrench k3_wrench = joints_[k3_jnt]->GetForceTorque(0);

        if (!hw_can_.status_idle_[fidx]) {
            if (finger_int_[fidx] > hw_can_.q_in_[k2_dof]) {
                finger_int_[fidx] -= hw_can_.v_in_[k2_dof] * vel_mult;
                is_opening = true;
                //if (getName() == "RightHand") {
                //    Logger::log() << Logger::Info << "op: " << finger_int_[fidx] << "  " << q_in_[k2_dof] << ", v: " << v_in_[k2_dof] << Logger::endl;
                //}
                if (finger_int_[fidx] <= hw_can_.q_in_[k2_dof]) {
                    hw_can_.status_idle_[fidx] = true;
                    Logger::log() << Logger::Info << "finger " << fidx << " idle -- opening" << Logger::endl;
                }
            }
            else {
                finger_int_[fidx] += hw_can_.v_in_[k2_dof] * vel_mult;
                is_opening = false;
                //if (getName() == "RightHand") {
                //    Logger::log() << Logger::Info << "cl: " << finger_int_[fidx] << "  " << q_in_[k2_dof] << ", v: " << v_in_[k2_dof] << Logger::endl;
                //}
                if (finger_int_[fidx] >= hw_can_.q_in_[k2_dof]) {
                    hw_can_.status_idle_[fidx] = true;
                    Logger::log() << Logger::Info << "finger " << fidx << " idle -- closing" << Logger::endl;
                }
            }

            double k2_angle = joints_[k2_jnt]->Position();
            double k3_angle = joints_[k3_jnt]->Position();

            double k3_angle_dest;
            double k2_angle_dest;
            k2_angle_dest = finger_int_[fidx];
            k3_angle_dest = finger_int_[fidx]/3;

            jc_->SetPositionTarget(joint_scoped_names_[k2_jnt], k2_angle_dest);
            jc_->SetPositionTarget(joint_scoped_names_[k3_jnt], k3_angle_dest);
        }
    }

/*
    for (int i = 0; i < 4; ++i) {
        if (hw_can_.move_hand_[i]) {
            hw_can_.move_hand_[i] = false;
            finger_int_[i] = getFingerAngle(i);
            hw_can_.status_idle_[i] = false;
            status_overcurrent_[i] = false;
            Logger::log() << Logger::Info <<  "move hand " << i << Logger::endl;
        }
    }

    if (!status_overcurrent_[3] && !hw_can_.status_idle_[3]) {
        // spread joints
        if (finger_int_[3] > hw_can_.q_in_[f1k1_dof_idx]) {
            finger_int_[3] -= hw_can_.v_in_[f1k1_dof_idx] * vel_mult;
            if (finger_int_[3] <= hw_can_.q_in_[f1k1_dof_idx]) {
                hw_can_.status_idle_[3] = true;
                Logger::log() << Logger::Info <<  "spread idle" << Logger::endl;
            }
        }
        else if (finger_int_[3] < hw_can_.q_in_[f1k1_dof_idx]) {
            finger_int_[3] += hw_can_.v_in_[f1k1_dof_idx] * vel_mult;
            if (finger_int_[3] >= hw_can_.q_in_[f1k1_dof_idx]) {
                hw_can_.status_idle_[3] = true;
                Logger::log() << Logger::Info <<  "spread idle" << Logger::endl;
            }
        }

        double f1k1_force = joints_[f1k1_jnt_idx]->GetForce(0);
        double f2k1_force = joints_[f2k1_jnt_idx]->GetForce(0);
        double spread_force = f1k1_force + f2k1_force;

        if (std::fabs(spread_force) > 0.5) {
            status_overcurrent_[3] = true;
            hw_can_.status_idle_[3] = true;
            Logger::log() << Logger::Info <<  "spread overcurrent" << Logger::endl;
            jc_->SetPositionTarget(joint_scoped_names_[f1k1_jnt_idx], mean_spread);
            jc_->SetPositionTarget(joint_scoped_names_[f2k1_jnt_idx], mean_spread);
        }
        else {
            if (!jc_->SetPositionTarget(joint_scoped_names_[f1k1_jnt_idx], finger_int_[3])) {
                Logger::log() << Logger::Warning <<  "jc_->SetPositionTarget(" << joint_scoped_names_[f1k1_jnt_idx] << ")" << Logger::endl;
            }
            if (!jc_->SetPositionTarget(joint_scoped_names_[f2k1_jnt_idx], finger_int_[3])) {
                Logger::log() << Logger::Warning <<  "jc_->SetPositionTarget(" << joint_scoped_names_[f2k1_jnt_idx] << ")" << Logger::endl;
            }
        }
    }

    // finger joints
    const int k2_dof_tab[3] = {0, 1, 2};
    const int k2_jnt_tab[3] = {1, 4, 6};
    const int k3_jnt_tab[3] = {2, 5, 7};
    for (int fidx = 0; fidx < 3; fidx++) {
        int k2_dof = k2_dof_tab[fidx];
        int k2_jnt = k2_jnt_tab[fidx];
        int k3_jnt = k3_jnt_tab[fidx];
        bool is_opening = false;
//        double k2_force = joints_[k2_jnt]->GetForce(0);
//        double k3_force = joints_[k3_jnt]->GetForce(0);
        gazebo::physics::JointWrench k2_wrench = joints_[k2_jnt]->GetForceTorque(0);
        gazebo::physics::JointWrench k3_wrench = joints_[k3_jnt]->GetForceTorque(0);

        if (!status_overcurrent_[fidx] && !hw_can_.status_idle_[fidx]) {
            if (finger_int_[fidx] > hw_can_.q_in_[k2_dof]) {
                finger_int_[fidx] -= hw_can_.v_in_[k2_dof] * vel_mult;
                is_opening = true;
                //if (getName() == "RightHand") {
                //    Logger::log() << Logger::Info << "op: " << finger_int_[fidx] << "  " << q_in_[k2_dof] << ", v: " << v_in_[k2_dof] << Logger::endl;
                //}
                if (finger_int_[fidx] <= hw_can_.q_in_[k2_dof]) {
                    hw_can_.status_idle_[fidx] = true;
                    Logger::log() << Logger::Info << "finger " << fidx << " idle -- opening" << Logger::endl;
                }
            }
            else {
                finger_int_[fidx] += hw_can_.v_in_[k2_dof] * vel_mult;
                is_opening = false;
                //if (getName() == "RightHand") {
                //    Logger::log() << Logger::Info << "cl: " << finger_int_[fidx] << "  " << q_in_[k2_dof] << ", v: " << v_in_[k2_dof] << Logger::endl;
                //}
                if (finger_int_[fidx] >= hw_can_.q_in_[k2_dof]) {
                    hw_can_.status_idle_[fidx] = true;
                    Logger::log() << Logger::Info << "finger " << fidx << " idle -- closing" << Logger::endl;
                }
            }

            double k2_angle = joints_[k2_jnt]->Position();
            double k3_angle = joints_[k3_jnt]->Position();

            //if (!is_opening && std::fabs(k2_force) > 0.25) {
            if (!is_opening && (k2_wrench.body1Force.Length() > 2.0 || k2_wrench.body1Torque.Length() > 1.0)) {
                clutch_break_angle_[fidx] = k2_angle;
                clutch_break_[fidx] = true;
                Logger::log() << Logger::Info << "finger " << fidx << " clutch is disengaged" << Logger::endl;
            }

            double k3_angle_dest;
            double k2_angle_dest;
//            if (std::fabs(k2_force) + std::fabs(k3_force) > 0.5) {
            if ((!is_opening && (k2_wrench.body1Force.Length() + k3_wrench.body1Force.Length() > 4.0 || k2_wrench.body1Torque.Length() + k2_wrench.body1Torque.Length() > 2.0)) ||
                (is_opening  && (k2_wrench.body1Force.Length() + k3_wrench.body1Force.Length() > 8.0 || k2_wrench.body1Torque.Length() + k2_wrench.body1Torque.Length() > 4.0))) {
                status_overcurrent_[fidx] = true;
                hw_can_.status_idle_[fidx] = true;
                Logger::log() << Logger::Info << "finger " << fidx << " overcurrent" << Logger::endl;
                k2_angle_dest = k2_angle;
                k3_angle_dest = k3_angle;
            }
            else if (clutch_break_[fidx]) {
                k3_angle_dest = finger_int_[fidx] - clutch_break_angle_[fidx] * 2.0 / 3.0;
                k2_angle_dest = clutch_break_angle_[fidx];
                if (is_opening && k3_angle_dest < clutch_break_angle_[fidx] / 3.0) {
                    k2_angle_dest = finger_int_[fidx];
                    k3_angle_dest = finger_int_[fidx]/3;
                    if (k2_angle < 0.03) {
                        clutch_break_[fidx] = false;
                        Logger::log() << Logger::Info << "finger " << fidx << " clutch is engaged" << Logger::endl;
                    }
                }
            }
            else {
                k2_angle_dest = finger_int_[fidx];
                k3_angle_dest = finger_int_[fidx]/3;
            }
            jc_->SetPositionTarget(joint_scoped_names_[k2_jnt], k2_angle_dest);
            jc_->SetPositionTarget(joint_scoped_names_[k3_jnt], k3_angle_dest);
        }

        // fingers may break if the force is too big
        if (too_big_force_counter_[fidx] < 100) {
            //gazebo::physics::JointWrench k2_wrench = joints_[k2_jnt]->GetForceTorque(0);
            //gazebo::physics::JointWrench k3_wrench = joints_[k3_jnt]->GetForceTorque(0);
            if (k2_wrench.body1Force.Length() > 200.0 || k2_wrench.body1Torque.Length() > 100.0 ||
                k3_wrench.body1Force.Length() > 200.0 || k3_wrench.body1Torque.Length() > 100.0) {
                too_big_force_counter_[fidx]++;
            }
            else {
                too_big_force_counter_[fidx] = 0;
            }
        }
        if (too_big_force_counter_[fidx] == 100) {
            jc_->SetPositionPID(joint_scoped_names_[k2_jnt], gazebo::common::PID());
            jc_->SetPositionPID(joint_scoped_names_[k3_jnt], gazebo::common::PID());
            Logger::log() << Logger::Error << "finger " << fidx << " is broken" << Logger::endl;
            too_big_force_counter_[fidx]++;
        }
    }
*/
    jc_->Update();

    data_valid_ = true;
}

