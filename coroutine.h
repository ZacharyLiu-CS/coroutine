//
//  coroutine.h
//  PROJECT coroutine
//
//  Created by zhenliu on 01/03/2022.
//  Copyright (c) 2022 zhenliu <liuzhenm@mail.ustc.edu.cn>.
//

#pragma once

#include <ucontext.h>
#include <memory>
#include <cstdint>
#include <vector>

typedef enum COROUTINE_STATUS{
  DEAD = 0,
  READY,
  RUNNING,
  SUSPEND
} COROUTINE_STATUS;


class Coroutine{
  private:

};
struct SCHEDULE_OPTIONS{
  int32_t capacity = 10;
  int32_t stack_size = 1024 * 1024;
};

struct SCHEDULE_STATUS{
  int32_t running_id = -1;
  int32_t coroutine_count = 0;
};

class Schedule{
  private:
    std::unique_ptr<char>  _stack;
    ucontext_t _main;
    SCHEDULE_OPTIONS _sched_options;
    SCHEDULE_STATUS _sched_status;
    std::vector<std::unique_ptr<Coroutine>> _co_list;
  public:
    Schedule(SCHEDULE_OPTIONS this_sched_options){
      _sched_options = this_sched_options;
      _stack = std::unique_ptr<char>(new char[_sched_options.stack_size]);

    }
};

