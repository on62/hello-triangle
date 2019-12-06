#ifndef GLOBAL_H
#define GLOBAL_H

#define NEW(type, count)     (type*)malloc(count * sizeof(type));

#define glCheck() assert(glGetError() == 0)

#endif // GLOBAL_H
