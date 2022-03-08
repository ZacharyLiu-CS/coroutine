//
//  coroutine.h
//  PROJECT coroutine
//
//  Created by zhenliu on 01/03/2022.
//  Copyright (c) 2022 zhenliu <liuzhenm@mail.ustc.edu.cn>.
//

#pragma once

#include <cstddef>
#include <ucontext.h>
#include <sys/ucontext.h>

#include <memory>
#include <cstdint>
#include <vector>

class Schedule;

typedef enum COROUTINE_STATUS{
  DEAD = 0,
  READY,
  RUNNING,
  SUSPEND
} COROUTINE_STATUS;

struct SCHEDULE_OPTIONS{
  int32_t capacity = 10;
  int32_t stack_size = 1024 * 1024;
};

struct SCHEDULE_STATUS{
  int32_t running_id = -1;
  int32_t coroutine_count = 0;
};


typedef void(* coroutine_func)(std::shared_ptr<Schedule> , std::shared_ptr<void>);

class Coroutine{
  private:
    coroutine_func _func;
    std::shared_ptr<void> _ud;
    std::shared_ptr<char> _stack;
    std::ptrdiff_t _size;
    std::ptrdiff_t _cap;
    COROUTINE_STATUS _coroutine_status;
    ucontext_t ctx;

  public:
    Coroutine(coroutine_func, std::shared_ptr<void>);
    Coroutine& operator=(const Coroutine &) = delete;
    ~Coroutine();
    void save_stack(std::shared_ptr<char>);
};

class Schedule{
  private:
    std::shared_ptr<char>  _stack;
    ucontext_t _main;
    SCHEDULE_OPTIONS _sched_options;
    SCHEDULE_STATUS _sched_status;
    std::vector<std::unique_ptr<Coroutine>> _co_list;
  public:
    Schedule(SCHEDULE_OPTIONS this_sched_options);
    int32_t go(coroutine_func, std::shared_ptr<void>);
    void resume(int32_t);
    void yield();
    COROUTINE_STATUS co_status(int32_t);
    SCHEDULE_STATUS sched_status();

};

