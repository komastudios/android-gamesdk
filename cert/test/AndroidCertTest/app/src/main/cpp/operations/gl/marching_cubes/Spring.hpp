/*
 * Copyright 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef spring_hpp
#define spring_hpp

#include <ancer/util/GLHelpers.hpp>

namespace marching_cubes {

template<typename T, glm::precision P>
class tspring3 {
 public:
  tspring3(T mass, T force, T damping, glm::tvec3<T, P> zero = {})
      : _mass(mass),
        _force(force),
        _damping(damping),
        _target(zero),
        _value(zero),
        _velocity(zero),
        _zero(zero) {
  }

  tspring3(const tspring3 &c)
      : _target(c._target),
        _value(c._value),
        _velocity(c._velocity),
        _zero(c._zero),
        _mass(c._mass),
        _force(c._force),
        _damping(c._damping) {
  }

  tspring3 &operator=(const tspring3 &rhs) {
    _mass = rhs._mass;
    _force = rhs._force;
    _damping = rhs._damping;
    _target = rhs._target;
    _value = rhs._value;
    _velocity = rhs._velocity;
    _zero = rhs._zero;

    return *this;
  }

  void setTarget(glm::tvec3<T, P> target) { _target = target; }

  glm::tvec3<T, P> target(void) const { return _target; }

  void setValue(glm::tvec3<T, P> v) {
    _value = v;
    _velocity = _zero;
  }

  glm::tvec3<T, P> value(void) const { return _value; }

  void setVelocity(glm::tvec3<T, P> vel) { _velocity = vel; }

  glm::tvec3<T, P> velocity(void) const { return _velocity; }

  bool converged(T epsilon = 0.001) const {
    return glm::distance(_velocity, _zero) <= epsilon && glm::distance(_value, _target) <= epsilon;
  }

  glm::tvec3<T, P> step(T deltaT) {
    if (deltaT < T(1e-4))
      return _value;

    auto error = _target - _value;
    auto dampingForce = _damping * _velocity;
    auto force = (error * _force) - dampingForce;
    auto acceleration = force / _mass;

    _velocity += acceleration * deltaT;
    _value += _velocity * deltaT;

    return _value;
  }

 private:
  T _mass, _force, _damping;
  glm::tvec3<T, P> _target, _value, _velocity, _zero;
};

typedef tspring3<float, glm::defaultp> spring3;
}

#endif