It's an asymmetric coroutine C++ library (support c11) inspired by @cloudwu.

You can create a schedule first by new a object `Schedule`, and then create coroutine in that schedule.

You should call `Schedule.go` to setup a coroutine in a schedule, within the coroutine, you can call `Schedule.yield` to swap out the cpu, corresponding to it is `Schedule.resume` to swap in the cpu.

Coroutines in the same schedule share the stack , so you can create many coroutines without worry about memory.

But switching context will copy the stack the coroutine used.

Code Exmaple (main.cc):
```
#include <iostream>
#include <memory>
#include "coroutine.h"

void foo(Schedule* s, std::shared_ptr<void> ud)
{
  auto argv = std::static_pointer_cast<int>(ud);
  std::cout << "argv: " << *argv << std::endl;
  for(int i = 0; i < *argv; ++i) {
    std::cout << "coroutine " << s ->sched_status().running_id << " : " << i << std::endl;
    s->yield();
  }
}

void test(Schedule& s)
{
  auto argv1 = std::make_shared<int>(1);
  auto argv2 = std::make_shared<int>(5);
  int32_t co1 = s.go(foo, argv1);
  int32_t co2 = s.go(foo, argv2);
  std::cout << "main start" << std::endl;
  while(s.co_status(co1) || s.co_status(co2)) {
    s.resume(co1);
    s.resume(co2);
  }

}

int main()
{
  SCHEDULE_OPTIONS sched_options;
  sched_options.capacity = 32;
  sched_options.stack_size = 1<< 20;
  Schedule s(sched_options);
  test(s);
  return 0;
}
```
Exmaple output:
```
main start
argv: 1
coroutine 0 : 0
argv: 5
coroutine 1 : 0
coroutine 1 : 1
coroutine 1 : 2
coroutine 1 : 3
coroutine 1 : 4
main end
```
