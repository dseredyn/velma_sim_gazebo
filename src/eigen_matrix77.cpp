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

#include "Eigen/Dense"
#include "common_behavior/abstract_port_converter.h"

class PortConverterEigen77ToArray : public common_behavior::Converter<Eigen::Matrix<double, 7, 7>, boost::array<double, 28 > > {
public:

    virtual void convert(const Eigen::Matrix<double, 7, 7> &from, boost::array<double, 28 > &to) const {
        for (int i = 0, idx = 0; i < 7; ++i) {
            for (int j = i; j < 7; ++j) {
                to[idx++] = from(i,j);
            }
        }
    }
};

class PortConverterArrayToEigen77 : public common_behavior::Converter<boost::array<double, 28 >, Eigen::Matrix<double, 7, 7> > {
public:

    virtual void convert(const boost::array<double, 28 > &from, Eigen::Matrix<double, 7, 7> &to) const {
        for (int i = 0, idx = 0; i < 7; ++i) {
            for (int j = i; j < 7; ++j) {
                to(i,j) = to(j,i) = from[idx++];
            }
        }
    }
};

REGISTER_PORT_CONVERTER(PortConverterEigen77ToArray);
REGISTER_PORT_CONVERTER(PortConverterArrayToEigen77);

