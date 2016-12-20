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

#ifndef __VELMA_CORE_VE_BODY_ABSTRACT_BEHAVIOR_H__
#define __VELMA_CORE_VE_BODY_ABSTRACT_BEHAVIOR_H__

#include "common_behavior/abstract_behavior.h"
#include "input_data.h"

namespace velma_core_ve_body_types {

class BehaviorBase : public common_behavior::BehaviorBase {
public:

    bool checkErrorCondition(
            const boost::shared_ptr<common_behavior::InputData >& in_data,
            const std::vector<RTT::TaskContext*> &components) const {
        return checkErrorCondition(boost::static_pointer_cast<InputData >(in_data), components);
    }

    bool checkStopCondition(
            const boost::shared_ptr<common_behavior::InputData >& in_data,
            const std::vector<RTT::TaskContext*> &components) const {
        return checkStopCondition(boost::static_pointer_cast<InputData >(in_data), components);
    }

    virtual bool checkErrorCondition(
            const boost::shared_ptr<InputData >& in_data,
            const std::vector<RTT::TaskContext*> &components) const = 0;

    virtual bool checkStopCondition(
            const boost::shared_ptr<InputData >& in_data,
            const std::vector<RTT::TaskContext*> &components) const = 0;

protected:
    BehaviorBase(const std::string& name, const std::string& short_name)
        : common_behavior::BehaviorBase(name, short_name)
    { }
};

};  // namespace velma_core_ve_body_types

#endif  // __VELMA_CORE_VE_BODY_ABSTRACT_BEHAVIOR_H__

