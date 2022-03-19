//
//  coroutine.h
//  PROJECT coroutine
//
//  Created by zhenliu on 01/03/2022.
//  Copyright (c) 2022 zhenliu <liuzhenm@mail.ustc.edu.cn>.
//

#pragma once

#include <cstddef>
#include <sys/ucontext.h>
#include <ucontext.h>

#include <cstdint>
#include <memory>
#include <vector>

class Schedule;

typedef enum COROUTINE_STATUS {
  DEAD = 0,
  READY,
  RUNNING,
  SUSPEND
} COROUTINE_STATUS;

struct SCHEDULE_OPTIONS {
  int32_t capacity = 16;
  int32_t stack_size = 1024 * 1024;
};

struct SCHEDULE_STATUS {
  int32_t running_id = -1;
  int32_t coroutine_count = 0;
};

typedef void (*coroutine_func)(Schedule*, std::shared_ptr<void>);

class Coroutine {
  private:
  // store the function pointer
  coroutine_func _func;
  // store the function argv
  std::shared_ptr<void> _ud;
  // coroutine separate stack (use to resume)
  std::shared_ptr<char> _local_stack;
  // stack size
  std::ptrdiff_t _size;
  // stack capacity
  std::ptrdiff_t _cap;
  // coroutine state
  COROUTINE_STATUS _coroutine_status;
  // context of coroutine
  ucontext_t _ctx;

  public:
  Coroutine(coroutine_func func = nullptr, std::shared_ptr<void> ud = nullptr);
  Coroutine& operator=(const Coroutine&) = delete;
  ~Coroutine();

  // save the shared stack to the separate stack of each coroutine
  // @param shared_stack_top: top of the shared stack (the stack is allocated at heap, so the addr is bottom)
  // @param size: size of the shared_stack
  void save_stack(char* shared_stack_top, int32_t size);

  void set_status(COROUTINE_STATUS coroutine_status) { _coroutine_status = coroutine_status; }

  COROUTINE_STATUS get_status() { return _coroutine_status; }

  ucontext_t* get_context() { return &_ctx; }

  std::ptrdiff_t get_size() { return _size; }

  char* get_stack() { return _local_stack.get(); }

  void run_func(Schedule* shed) { _func(shed, _ud); }
};

class Schedule {
  private:
  std::shared_ptr<char> _shared_stack;
  ucontext_t _main;
  SCHEDULE_OPTIONS _sched_options;
  SCHEDULE_STATUS _sched_status;
  std::vector<std::shared_ptr<Coroutine>> _co_list;

  public:
  Schedule(SCHEDULE_OPTIONS sched_options);
  ~Schedule();
  int32_t go(coroutine_func, std::shared_ptr<void>);
  void resume(int32_t);
  void yield();

  static void main_func(uint32_t, uint32_t);
  void set_runningid(int32_t id) { _sched_status.running_id = id; }
  void decrease_coroutine(int32_t id)
  {
    _sched_status.coroutine_count--;
    _co_list[id].reset();
  }

  COROUTINE_STATUS co_status(int32_t id)
  {
    if (_co_list[id] != nullptr)
      return _co_list[id]->get_status();
    else
      return COROUTINE_STATUS::DEAD;
  }
  SCHEDULE_STATUS& sched_status() { return _sched_status; }
  std::shared_ptr<Coroutine> get_coroutine(int32_t id) { return _co_list[id]; }
};
