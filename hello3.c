#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>

struct time_record {
  struct list_head node;
  ktime_t start;
  ktime_t end;
};

MODULE_AUTHOR("IO-11 Kudelia Sasha");
MODULE_DESCRIPTION("AK-11 Kudelia Sasha");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION("3.0");

static LIST_HEAD(time_records);

static uint custom_param = 1;
module_param(custom_param, uint, 0444);
MODULE_PARM_DESC(custom_param, "Custom Parameter (p)");

static int __init secret_init(void)
{
  uint i = 0;
  struct time_record *record;

  pr_info("Custom Parameter: %d\n", custom_param);

  if (!custom_param) {
    pr_warn("The custom parameter is 0\n");
    return 0;
  }

  if (custom_param >= 5 && custom_param <= 10) {
    pr_warn("The custom parameter is %d, between 5 and 10. Enter a number less than 5\n", custom_param);
    return 0;
  }

  BUG_ON(custom_param > 10);

  for (i = 0; i < custom_param; i++) {
    record = kmalloc(sizeof(struct time_record), GFP_KERNEL);

    if (i == 5)
      record = NULL;

    if (ZERO_OR_NULL_PTR(record))
      goto exception;

    record->start = ktime_get();
    pr_info("Hello, World!\n");
    record->end = ktime_get();

    list_add_tail(&(record->node), &time_records);
  }

  return 0;

exception:
  pr_err("The end of memory...\n");
  list_for_each_entry_safe(record, record, &time_records, node) {
    list_del(&record->node);
    kfree(record);
  }

  BUG();
  return -ENOMEM;
}

static void __exit program_exit(void)
{
  struct time_record *record, *tmp;

  list_for_each_entry_safe(record, tmp, &time_records, node) {
    pr_info("Time: %lld",
            record->end - record->start);

    list_del(&record->node);
    kfree(record);
  }

  BUG_ON(!list_empty(&time_records));
}

module_init(program_init);
module_exit(program_exit);