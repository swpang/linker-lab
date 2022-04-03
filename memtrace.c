//------------------------------------------------------------------------------
//
// memtrace
//
// trace calls to the dynamic memory manager
//
#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memlog.h>
#include <memlist.h>

//
// function pointers to stdlib's memory management functions
//
static void *(*mallocp)(size_t size) = NULL;
static void (*freep)(void *ptr) = NULL;
static void *(*callocp)(size_t nmemb, size_t size);
static void *(*reallocp)(void *ptr, size_t size);

//
// statistics & other global variables
//
static unsigned long n_malloc  = 0; /* number of malloc calls */
static unsigned long n_calloc  = 0; /* number of calloc calls */
static unsigned long n_realloc = 0; /* number of realloc calls */
static unsigned long n_allocb  = 0; /* number of alloc blocks */
static unsigned long n_freeb   = 0; /* number of free blocks */
static item *list = NULL;

//
// init - this function is called once when the shared library is loaded
//
__attribute__((constructor))
void init(void)
{
  char *error;

  LOG_START();

  // initialize a new list to keep track of all memory (de-)allocations
  // (not needed for part 1)
  list = new_list();

  /* Assign malloc function to mallocp pointer */
  if (!mallocp) {
    mallocp = dlsym(RTLD_NEXT, "malloc");
    if ((error = dlerror()) != NULL) {
      fputs(error, stderr);
      exit(1);
    }
  }

	/* Assign free function to freep pointer */
  if (!freep) {
    freep = dlsym(RTLD_NEXT, "free");
    if ((error = dlerror()) != NULL) {
      fputs(error, stderr);
      exit(1);
    }
  }

	/* Assign calloc funtion to callocp pointer */
	if (!callocp) {
		callocp = dlsym(RTLD_NEXT, "calloc");
		if ((error = dlerror()) != NULL) {
			fputs(error, stderr);
			exit(1);
		}
	}

	/* Assign realloc function to reallocp pointer */
	if (!reallocp) {
		reallocp = dlsym(RTLD_NEXT, "realloc");
		if ((error = dlerror()) != NULL) {
			fputs(error, stderr);
			exit(1);
		}
	}
}

//
// fini - this function is called once when the shared library is unloaded
//
__attribute__((destructor))
void fini(void)
{
	unsigned long tot_malloc = n_malloc + n_realloc + n_calloc;
	unsigned long avg_malloc = tot_malloc / n_allocb;
	
	LOG_STATISTICS(tot_malloc, avg_malloc, n_freeb);
	
	int temp = 0;
	item *next;
	while (list) {
		if ((temp == 0) && (list->cnt > 0)) {
			LOG_NONFREED_START();
			temp = 1;
		}
		if (list->cnt > 0) {
			LOG_BLOCK(list->ptr, list->size, list->cnt);
		}
		list = list->next;
	}

	LOG_STOP();

  	// free list (not needed for part 1)
  	free_list(list);
}

// ...
//
void *malloc(size_t size) {
	void *ptr;
	item* temp;

	ptr = mallocp(size);
	
	temp = alloc(list, ptr, size);
	n_malloc = n_malloc + (uint) size;
	n_allocb++;

	LOG_MALLOC(size, ptr);
	
	return ptr;
}

void free(void *ptr) {
	item* temp;

	temp = find(list, ptr);
	LOG_FREE(ptr);

	if (temp == NULL) {
		LOG_ILL_FREE();
	} else {
		item* de_temp = dealloc(list, ptr);
		if (de_temp->cnt < 0) {
			LOG_DOUBLE_FREE();
		} else {
			freep(ptr);
			n_freeb = n_freeb + temp->size;
			temp = NULL;
		}
	}
}

void* calloc(size_t nmemb, size_t size) {
	void *ptr;
	item* temp;

	ptr = callocp(nmemb, size);

	temp = alloc(list, ptr, size);
	n_calloc = n_calloc + (uint) size;
	n_allocb++;

	LOG_CALLOC(nmemb, size, ptr);

	return ptr;
}

void* realloc(void* ptr, size_t size) {
	void* res;
	item* temp;

	temp = find(list, ptr);
	res = reallocp(ptr, size);
	n_freeb = n_freeb + temp->size;
	dealloc(list, ptr);

	temp = alloc(list, res, size);
	n_realloc = n_realloc + (uint) size;
	n_allocb++;

	LOG_REALLOC(ptr, size, res);

	return res;
}
