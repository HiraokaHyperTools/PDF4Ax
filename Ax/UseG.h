
#pragma once

template<typename T> class UseGFree {
	UseGFree(const UseGFree &s);
	void operator =(const UseGFree &s);
public:
	T *ptr;
	UseGFree<T>(T *ptr = NULL): ptr(ptr) {
	}
	~UseGFree() {
		g_free(ptr);
	}
	operator T *() {
		return ptr;
	}
	T **operator &() {
		return &ptr;
	}
};
class UseGErrorFree {
	UseGErrorFree(const UseGErrorFree &s);
	void operator =(const UseGErrorFree &s);
public:
	GError *ptr;
	UseGErrorFree(GError *ptr = NULL): ptr(ptr) {
	}
	~UseGErrorFree() {
		g_error_free(ptr);
	}
	operator GError *() {
		return ptr;
	}
	GError **operator &() {
		return &ptr;
	}
};
template<typename T> class UseGObjectUnref {
	UseGObjectUnref(const UseGObjectUnref &s);
	void operator =(const UseGObjectUnref &s);
public:
	T *ptr;
	UseGObjectUnref(T *ptr = NULL): ptr(ptr) {
	}
	~UseGObjectUnref() {
		Release();
	}
	operator T *() {
		return ptr;
	}
	T **operator &() {
		return &ptr;
	}
	void Release() {
		if (ptr != NULL) {
			g_object_unref(ptr);
			ptr = NULL;
		}
	}
	void Attach(T *pNew) {
		Release();
		ptr = pNew;
	}
};

class UseCairoDestroy {
	UseCairoDestroy(const UseCairoDestroy &s);
	void operator =(const UseCairoDestroy &s);
public:
	typedef cairo_t T;
	T *ptr;
	UseCairoDestroy(T *ptr = NULL): ptr(ptr) {
	}
	~UseCairoDestroy() {
		Release();
	}
	operator T *() {
		return ptr;
	}
	void Release() {
		if (ptr != NULL) {
			cairo_destroy(ptr);
			ptr = NULL;
		}
	}
};

class UseCairoSurfaceDestroy {
	UseCairoSurfaceDestroy(const UseCairoSurfaceDestroy &s);
	void operator =(const UseCairoSurfaceDestroy &s);
public:
	typedef cairo_surface_t T;
	T *ptr;
	UseCairoSurfaceDestroy(T *ptr = NULL): ptr(ptr) {
	}
	~UseCairoSurfaceDestroy() {
		Release();
	}
	operator T *() {
		return ptr;
	}
	void Release() {
		if (ptr != NULL) {
			cairo_surface_destroy(ptr);
			ptr = NULL;
		}
	}
};
