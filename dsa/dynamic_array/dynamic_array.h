#ifndef _SWISS_DYNAMIC_ARR
#define _SWISS_DYNAMIC_ARR

// Example:
// ===============
/*
typedef struct {
	int* items;
	int capacity;
	int size;
} Array;

int main(){
	Array arr;
	dyarr_init(&arr, 3);
	dyarr_append(&arr, 2);
	dyarr_append(&arr, 8);
	dyarr_append(&arr, 4);
	dyarr_append(&arr, 9);
	dyarr_remove(&arr, 0);

	printf("%d Index\n", dyarr_find(&arr, 4));

	for(int i = 0; i < arr.size; i++){
		printf("%d\n", arr.items[i]);
	}

	dyarr_foreach(&arr, val, {
		printf("%d Nice\n", val * 2);
	});
	
}
*/


#define DYARR_INIT_CAP 10

#define dyarr_init(arr, size_init)										\
	do {																\
		(arr)->items = malloc((size_init) * (sizeof(*(arr)->items)));	\
		(arr)->capacity = size_init;									\
		(arr)->size = 0;												\
	} while (0)															\


#define dyarr_free(arr)					\
	do {								\
		if((arr)->items)){				\
			free((arr)->items);			\
			(arr)->items = NULL;		\
			(arr)->size = 0;			\
			(arr)->capacity = 0;		\
		}								\
	} while (0)							\

#define dyarr_append(arr, item)																	\
	do {																						\
		if((arr)->size >= (arr)->capacity){														\
			(arr)->capacity = (arr)->capacity == 0 ? DYARR_INIT_CAP : (arr)->capacity*2;		\
			(arr)->items = realloc((arr)->items, (arr)->capacity * (sizeof(*(arr)->items)));	\
		}																						\
		(arr)->items[(arr)->size] = (item);														\
		(arr)->size++;																			\
																								\
	} while (0)																					\


#define dyarr_size(arr) ((arr)->size)

#define dyarr_capacity(arr) ((arr)->capacity)

#define dyarr_get(arr, index) ((arr)->items[(index)])

#define dyarr_remove(arr, index)								\
	do {														\
		if((index) >= (0) && (index) < (arr)->size){			\
			for(int _i = (index); _i < (arr)->size - 1; _i++){	\
				(arr)->items[_i] = (arr)->items[_i + 1];		\
			}													\
			(arr)->size--;										\
		}														\
	} while (0)													\




#define dyarr_contains(arr, item) ({					\
	int _found = 0;										\
	for (int _i = 0; _i < (arr)->size; _i++) {			\
		if ((arr)->items[_i] == (item)) {				\
			_found = 1;									\
			break;										\
		}												\
	}													\
	_found;												\
})


#define dyarr_find(arr, item) ({				\
	int _index = -1;							\
	for(int _i = 0; _i < (arr)->size; _i++){	\
		if((arr)->items[_i] == (item)){			\
			_index = _i;						\
			break;								\
		}										\
	}											\
	_index;										\
})												\

// #define dyarr_copy(dest, src)		
#define dyarr_resize(arr, new_capacity)														\
	do {																					\
		if((new_capacity) > (arr)->capacity){												\
			(arr)>items = realloc((arr)->items, (new_capacity) * (sizeof(*(arr)->items)));	\
			(arr)->capacity = (new_capacity);												\
		}																					\
	} while (0)																				\



//#define dyarr_swap(arr, index1, index2)		
#define dyarr_foreach(arr, var, code)				\
	do {											\
		for (int _i = 0; _i < (arr)->size; _i++) {	\
			int var = (arr)->items[_i];				\
			code;									\
		}											\
	} while (0)										\







#endif //_SWISS_DYNAMIC_ARR
