#ifndef _MODULE_H_
#define _MODULE_H_

#include "c99defs.h"

#if defined(_DO_NOT_EXPORT)
#define MODULE_EXPORT
#else
#ifdef __cplusplus
#define MODULE_EXPORT extern "C" EXPORT
#else
#define MODULE_EXPORT EXPORT
#endif
#endif

#define LIBSGL_API_MAJOR_VER 0
#define LIBSGL_API_MINOR_VER 1
#define LIBSGL_API_PATCH_VER 1
#define MAKE_SEMANTIC_VERSION(major, minor, patch) \
                             ((major << 24) | \
                              (minor << 16) | \
                               patch        )
#define LIBSGL_API_VER \
	MAKE_SEMANTIC_VERSION(LIBSGL_API_MAJOR_VER, \
                              LIBSGL_API_MINOR_VER, \
                              LIBSGL_API_PATCH_VER)

typedef struct sgl_module sgl_module_t;

struct sgl_module{
	char *mod_name;
	const char *file;
	char *bin_path;
	char *data_path;
	void *module;
	bool loaded;

	bool        (*load)(void);
	void        (*unload)(void);
	uint32_t    (*ver)(void);
	void        (*set_pointer)(sgl_module_t *module);

	struct sgl_module *next;
};

#define SGL_DECLARE_MODULE() \
	static sgl_module_t *sgl_module_pointer; \
	MODULE_EXPORT void sgl_module_set_pointer(sgl_module_t *module); \
	void sgl_module_set_pointer(sgl_module_t *module) \
	{ \
		sgl_module_pointer = module; \
	} \
	sgl_module_t *sgl_current_module(void) {return sgl_module_pointer;} \
	MODULE_EXPORT uint32_t sgl_module_ver(void); \
	uint32_t sgl_module_ver(void) {return LIBSGL_API_VER;}

MODULE_EXPORT bool obs_module_load(void);
MODULE_EXPORT void obs_module_unload(void);

#endif