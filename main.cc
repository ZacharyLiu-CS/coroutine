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

struct message_list{
  int32_t input;
  int32_t output;
  message_list(int i, int o): input(i), output(o){}
};
void generator(Schedule *s, std::shared_ptr<void> ud) {
  auto argv = std::static_pointer_cast<message_list>(ud);
  for (int i = 0; i < argv->input; ++i) {
    argv->output = i;
    s->yield();
  }
}

void test(Schedule &s) {
  auto argv1 = std::make_shared<message_list>(5,0);
  int32_t co1 = s.go(generator, argv1);
  std::cout << "main start" << std::endl;
  while (s.co_status(co1) ) {
    s.resume(co1);
    std::cout << "coroutine "<< co1 << " output: " << argv1->output << std::endl;
  }

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
