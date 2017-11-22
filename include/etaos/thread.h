/*
 *  ETA/OS - Thread header
 *  Copyright (C) 2014, 2015, 2016, 2017  Michel Megens <dev@bietje.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file etaos/thread.h
 */

#ifndef __THREAD_H__
#define __THREAD_H__

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/spinlock.h>
#include <etaos/list.h>

/**
 * @addtogroup thread
 */
/** @{ */

struct sched_class;

CDECL
extern struct sched_class fifo_class;
#ifdef CONFIG_THREAD_QUEUE
/**
 * @brief Define a new thread queue.
 * @param __name Name of the thread queue.
 */
#define DEFINE_THREAD_QUEUE(__name)			 \
	struct thread_queue __name = {		 	 \
		.sched_class = &fifo_class,	 \
		.lock = STATIC_SPIN_LOCK_INIT,		 \
		.qhead = SIGNALED,			 \
	}

/**
 * @brief Initialise a new thread queue.
 *
 * The sched_class, lock and qhead members will be initialised by this
 * macro.
 */
#define INIT_THREAD_QUEUE {			 \
		.sched_class = &fifo_class, \
		.lock = STATIC_SPIN_LOCK_INIT,	 \
		.qhead = SIGNALED,		 \
	}

#endif

CDECL_END

/**
 * @brief Thread handle type definition.
 * @param arg Thread argument.
 */
typedef void (*thread_handle_t)(void *arg);

/**
 * @brief Define a thread.
 * @param fn Function name of the thread.
 * @param param Name of the thread function parameter.
 *
 * The function will be defined ass:
   @code{.c}
   static void fn(void *param)
   @endcode
 */
#define THREAD(fn, param) \
static void fn(void * param); \
static void fn(void *param)

#ifdef CONFIG_LOTTERY
struct lottery_ticket {
	struct list_head list;
	unsigned short ticket;
};
#endif

/**
 * @struct rr_entity
 * @brief Round robin entity.
 */
struct rr_entity {
	struct thread *next; //!< List entry pointer.
#ifdef CONFIG_LOTTERY
	struct list_head tickets; //!< Lotery tickets.
#endif
#ifdef CONFIG_EDF
	time_t deadline; //!< EDF deadline timestamp.
#endif
};

#ifdef CONFIG_THREAD_QUEUE
/**
 * @struct thread_queue
 * @brief Thread queue descriptor
 */
struct thread_queue {
	/**
	 * @brief Sched class organizing the queue.
	 */
	struct sched_class *sched_class;
	/**
	 * @brief queue lock.
	 */
	spinlock_t lock;
	/**
	 * @brief Head of the queue.
	 */
	struct thread *qhead;
};

#endif

struct stack_info {
	void *base; //!< Stack base pointer.
	stack_t *sp; //!< Stack pointer.
	size_t size; //!< Stack size.
#ifdef CONFIG_STACK_TRACE_LENGTH
	size_t max_length;
#endif
};

struct rq;
/**
 * @struct thread
 * @brief Thread descriptor.
 * @ingroup thread
 *
 * All information available for a thread is stored in this structure.
 */
struct thread {
	struct thread *volatile*queue; //!< Queue root pointer.
	struct list_head entry; //!< Wake / kill queue entry.

#ifdef CONFIG_PREEMPT
	unsigned int slice; //!< Time slice.
	int preempt_cnt; //!< Preempt enable counter.
#endif

	unsigned long cpu_state; //!< CPU flags for this thread.
	unsigned long irq_state; //!< IRQ state of this thread.

	bool on_rq; //!< Run queue enable.
	struct rq *rq; //!< Run queue pointer.
	unsigned long flags; //!< Thread flags

#ifdef CONFIG_SCHED_FAIR
	time_t cputime; //!< Total CPU time.
#endif

	struct stack_info stack;
	unsigned char prio; //!< Thread priority.
#ifdef CONFIG_DYN_PRIO
	unsigned char dprio; //!< Dynamic thread priority.
#endif
	struct timer *timer; //!< Event timer.

#ifdef CONFIG_EVENT_MUTEX
	unsigned char ec; //!< Event counter.
#endif

	void *param; //!< Thread parameter.
#if defined(CONFIG_RR) || defined(CONFIG_FIFO) || defined(CONFIG_LOTTERY)
	struct rr_entity se; //!< Scheduling entity.
#endif
#ifdef CONFIG_EXTENDED_THREAD
	struct thread_queue joinq; //!< Waiting queue for \p join.
#endif
	char name[16]; //!< Name of the thread.
};

/**
 * @brief Thread attributes
 */
typedef struct thread_attr {
	size_t stack_size; //!< Size of the stack.
	void *stack; //!< Stack base pointer.
	unsigned char prio; //!< Thread priority.
} thread_attr_t;

/**
 * @name Thread flags
 * @{
 */
#define THREAD_RUNNING_FLAG	 0 //!< Thread running flag.
#define THREAD_SLEEPING_FLAG	 1 //!< Thread sleeping flag.
#define THREAD_WAITING_FLAG	 2 //!< Thread waiting flag.
#define THREAD_EXIT_FLAG 	 3 //!< Thread waiting to be killed.
#define THREAD_NEED_RESCHED_FLAG 4 //!< Thread need a resched.
#define PREEMPT_NEED_RESCHED_FLAG 5 //!< Thread has used its full time slice.
#define THREAD_IDLE_FLAG	 6 //!< Thread is the idle thread.
#define THREAD_SYSTEM_STACK      7 //!< Stack is allocated by the system.
#define THREAD_REMOTE_KILL	 8 //!< Thread has been killed by someone else
/** @} */

/**
 * @name Thread priority's
 * @{
 */
#define SCHED_DEFAULT_PRIO	120 //!< Default 'avarage' priority.
#define SCHED_HIGH_PRIO		 40 //!< Standard high priority.
#define SCHED_LOW_PRIO		200 //!< Standard low priority.
#define SCHED_HIGHEST_PRIO	  0 //!< Highest possible priority.
#define SCHED_LOWEST_PRIO	255 //!< Lowest possible priority.
/** @} */

CDECL
extern int thread_initialise(struct thread *tp, const char *name,
		thread_handle_t handle, void *arg, size_t stack_size,
		void *stack, unsigned char prio);
extern int thread_init(struct thread *tp, const char *name, thread_handle_t handle,
		void *arg, thread_attr_t *attr);
extern struct thread *thread_create(const char *name, thread_handle_t handle,
		void *arg, thread_attr_t *attr);
extern struct thread *thread_alloc(const char *name, thread_handle_t handle,
		void *arg, thread_attr_t *attr);
extern void thread_start(struct thread *tp);

extern void sched_init_idle(struct thread *tp, const char *name,
		thread_handle_t handle, void *arg, size_t stack_size,
		void *stack);

#ifdef CONFIG_STACK_TRACE_LENGTH
static inline size_t thread_max_stack_length(struct thread *tp)
{
	return tp->stack.max_length;
}

static inline float thread_max_relative_stack_length(struct thread *tp)
{
	float m, s;

	m = tp->stack.max_length;
	s = tp->stack.size;

	return m / s * 100.0f;
}
#else
static inline size_t thread_max_stack_length(struct thread *tp)
{
	return 0UL;
}

static inline float thread_max_relative_stack_length(struct thread *tp)
{
	return -1.0f;
}
#endif

extern struct thread *current_thread();

extern int thread_destroy(struct thread *tp);
extern int thread_destroy_by_name(const char *name);
extern void yield(void);
extern void sleep(unsigned ms);
extern void kill(void);
extern void wait(void);
extern void signal(struct thread *tp);
extern unsigned char nice(unsigned char prio);

#ifdef CONFIG_EXTENDED_THREAD
extern int join(struct thread *tp);
#endif

#ifdef CONFIG_THREAD_QUEUE
extern void thread_queue_init(struct thread_queue *qp);
#endif

#ifdef CONFIG_SCHED_DBG
extern void print_rq(void);
#endif

CDECL_END

#endif /* __THREAD_H__ */

/** @} */
