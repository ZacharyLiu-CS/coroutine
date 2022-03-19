//
//  main.cc
//  PROJECT main
//
//  Created by zhenliu on 01/03/2022.
//  Copyright (c) 2022 zhenliu <liuzhenm@mail.ustc.edu.cn>.
//

#include "coroutine.h"
#include <iostream>
#include <memory>

void foo(Schedule *s, std::shared_ptr<void> ud) {
  auto argv = std::static_pointer_cast<int>(ud);
  std::cout << "argv: " << *argv << std::endl;
  for (int i = 0; i < *argv; ++i) {
    std::cout << "coroutine " << s->sched_status().running_id << " : " << i
              << std::endl;
    s->yield();
  }
}

void test(Schedule &s) {
  auto argv1 = std::make_shared<int>(1);
  auto argv2 = std::make_shared<int>(5);
  int32_t co1 = s.go(foo, argv1);
  int32_t co2 = s.go(foo, argv2);
  std::cout << "main start" << std::endl;
  while (s.co_status(co1) || s.co_status(co2)) {
    s.resume(co1);
    s.resume(co2);
  }
  //  while( s.co_status(co2) != COROUTINE_STATUS::DEAD) {
  //   }
  std::cout << "main end" << std::endl;
}

int main() {
  SCHEDULE_OPTIONS sched_options;
  sched_options.capacity = 32;
  sched_options.stack_size = 1 << 20;
  Schedule s(sched_options);
  test(s);
  return 0;
}
