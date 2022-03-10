//
//  coroutine.cc
//  PROJECT coroutine
//
//  Created by zhenliu on 01/03/2022.
//  Copyright (c) 2022 zhenliu <liuzhenm@mail.ustc.edu.cn>.
//

#include "coroutine.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <memory>
#include <cstring>
#include <sys/ucontext.h>
#include <type_traits>
#include <ucontext.h>

Coroutine::Coroutine(coroutine_func func, std::shared_ptr<void> ud):
  _func(func), _ud(ud), _stack(nullptr), _size(0), _cap(0),
  _coroutine_status(COROUTINE_STATUS::READY){
  }

Coroutine::~Coroutine(){
}

void Coroutine::save_stack(char* top, int32_t size){
  char dummy = 0;
  assert(top - &dummy <= size);
  if( _cap < top - &dummy ){
    _stack.reset();
    _cap = top - &dummy ;
    _stack = std::shared_ptr<char>(new char[_cap]);
  }
  _size = top - &dummy;
  memcpy(_stack.get(), &dummy, _size);

}

Schedule::Schedule(SCHEDULE_OPTIONS sched_options): _sched_options(sched_options){
  _stack = std::shared_ptr<char>(new char[_sched_options.stack_size]);
  _co_list.resize(_sched_options.capacity);
  std::fill(_co_list.begin(), _co_list.end(), nullptr);
}

Schedule::~Schedule(){
  _co_list.clear();
  _sched_status.coroutine_count = 0;
  _sched_status.running_id = -1;
}
int32_t Schedule::go(coroutine_func func, std::shared_ptr<void> ud){
  auto co = std::make_shared<Coroutine>(func, ud);
  if( _sched_status.coroutine_count >= _sched_options.capacity ){
    int32_t id = _sched_options.capacity;
    _co_list.resize(_sched_options.capacity*2);
    _co_list[id] = co;
    _sched_options.capacity *= 2;
    _sched_status.coroutine_count += 1;
    return id;
  }else{
    for(auto i = 0; i< _sched_options.capacity; i++ ){
      int32_t id = (i + _sched_status.coroutine_count) % _sched_options.capacity;
      if ( _co_list[id] == nullptr ){
        _co_list[id] = co;
        _sched_status.coroutine_count += 1;
        return id;
      }
    }
  }
  assert(0);
  return -1;
}

void Schedule::resume(int32_t id){
  assert(_sched_status.running_id == -1);
  assert(id >= 0 && id < _sched_options.capacity);
  auto co = _co_list[id];
  if (co == nullptr){
    return;
  }
  auto status = co->get_status();
  switch(status){
    case COROUTINE_STATUS::READY:{
                                   ucontext_t* co_ctx = co->get_context();
                                   getcontext(co_ctx);
                                   co_ctx->uc_stack.ss_sp = _stack.get();
                                   co_ctx->uc_stack.ss_size = _sched_options.stack_size;
                                   co_ctx->uc_link = &_main;
                                   _sched_status.running_id = id;
                                   uintptr_t ptr = (uintptr_t)this;
                                   co->set_status(COROUTINE_STATUS::RUNNING);
                                   makecontext(co_ctx, (void (*)(void)) main_func, 2, (uint32_t)ptr, (uint32_t)(ptr >> 32));
                                   swapcontext(&_main, co_ctx);
                                   break;
                                 }
    case COROUTINE_STATUS::SUSPEND:{
                                     memcpy(_stack.get() + _sched_options.stack_size - co->get_size(), co->get_stack(), co->get_size());
                                     _sched_status.running_id = id;
                                     swapcontext(&_main, co->get_context());
                                     break;
                                   }
    default:
                                   return;
  }
}
void Schedule::yield(){
  assert(_sched_status.running_id >= 0);
  auto co = _co_list[_sched_status.running_id];
  co->save_stack(_stack.get() + _sched_options.stack_size, _sched_options.stack_size);
  co->set_status(COROUTINE_STATUS::SUSPEND);
  _sched_status.running_id = -1;
  swapcontext(co->get_context(), &_main);
}
void Schedule::main_func(uint32_t low32, uint32_t high32){
  uintptr_t sptr = (uintptr_t)low32 | ((uintptr_t)high32 << 32);
  Schedule* sched = (Schedule*)sptr;
  int32_t id = sched->sched_status().running_id;
  auto co = sched->get_coroutine(id);
  co->run_func(sched);
  sched->set_runningid(-1);
  sched->decrease_coroutine(id);
}
