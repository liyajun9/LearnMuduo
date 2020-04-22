//
// Created by ralph on 3/31/20.
//

#ifndef LEARNMUDUO_ENDIAN_H
#define LEARNMUDUO_ENDIAN_H

#include <cstdint>
#include <endian.h>

namespace ynet{

//assemble inline functions
inline uint64_t hostToNetwork64(uint64_t host64){
    return htobe64(host64);
}

inline uint32_t hostToNetwork32(uint32_t host32){
    return htobe32(host32);
}

inline uint16_t hostToNetwork16(uint16_t host16){
    return htobe16(host16);
}

inline uint64_t networkToHost64(int64_t network64){
    return be64toh(network64);
}

inline uint32_t networkToHost32(int32_t network32){
    return be32toh(network32);
}

inline uint16_t networkToHost16(int16_t network16){
    return be16toh(network16);
}

}
#endif //LEARNMUDUO_ENDIAN_H
