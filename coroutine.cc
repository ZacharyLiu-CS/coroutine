//
//  coroutine.cc
//  PROJECT coroutine
//
//  Created by zhenliu on 01/03/2022.
//  Copyright (c) 2022 zhenliu <liuzhenm@mail.ustc.edu.cn>.
//

#include "coroutine.h"
#include <cassert>
#include <cstdint>
#include <memory>

Coroutine::Coroutine(coroutine_func func, std::shared_ptr<void> ud):
  _func(func), _ud(ud), _stack(nullptr), _size(0), _cap(0),
  _coroutine_status(COROUTINE_STATUS::READY){
}

Coroutine::~Coroutine(){
}

void Coroutine::save_stack(std::shared_ptr<char> top){
  char dummy = 0;
  assert(static_cast<std::uint32_t>(top.get() - &dummy) <= _size);
  
}
