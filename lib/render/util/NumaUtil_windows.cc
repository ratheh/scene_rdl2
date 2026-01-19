// Copyright 2024-2025 DreamWorks Animation LLC
// SPDX-License-Identifier: Apache-2.0

// Windows stub implementation of NumaUtil
// NUMA APIs are Linux-specific, so we provide simplified stubs on Windows

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "NumaUtil.h"

#include <cstdlib>
#include <sstream>

namespace scene_rdl2 {

//-----------------------------------------------------------------------------
// NumaNode stubs
//-----------------------------------------------------------------------------

NumaNode::NumaNode(const unsigned nodeId,
                   const unsigned totalNode,
                   const size_t memSize,
                   const std::vector<unsigned>& cpuIdList,
                   const std::vector<int>& nodeDistance)
    : mNodeId{nodeId}
    , mTotalNode{totalNode}
    , mMemSize{memSize}
    , mPageSize{4096}  // Standard page size
    , mCpuIdList{cpuIdList}
    , mNodeDistance{nodeDistance}
{
}

void*
NumaNode::alloc(const size_t size) const
{
    // On Windows, just use aligned_alloc for cache-line aligned allocation
    return _aligned_malloc(size, 64);  // 64-byte cache line alignment
}

void
NumaNode::free(void* const memory, const size_t size) const
{
    _aligned_free(memory);
}

bool
NumaNode::isBelongMem(void* const memory, const size_t size) const
{
    // On Windows without NUMA support, all memory "belongs" to node 0
    return (mNodeId == 0);
}

bool
NumaNode::isBelongCpu(const unsigned cpuId) const
{
    if (mCpuIdList.empty()) return false;
    for (unsigned id : mCpuIdList) {
        if (id == cpuId) return true;
    }
    return false;
}

bool
NumaNode::alignmentSizeCheck(const size_t alignment) const
{
    if (mPageSize < alignment) return false;
    return ((mPageSize % alignment) == 0);
}

std::string
NumaNode::show() const
{
    std::ostringstream ostr;
    ostr << "NumaNode (Windows stub) { nodeId:" << mNodeId << " }";
    return ostr.str();
}

//-----------------------------------------------------------------------------
// NumaUtil stubs
//-----------------------------------------------------------------------------

NumaUtil::NumaUtil()
{
    // On Windows, create a single virtual NUMA node representing all CPUs
    std::vector<unsigned> cpuIdList;

    // Get number of processors
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    unsigned numCpus = sysInfo.dwNumberOfProcessors;

    for (unsigned i = 0; i < numCpus; ++i) {
        cpuIdList.push_back(i);
    }

    // Single node with distance 10 to itself (standard NUMA convention)
    std::vector<int> nodeDistance = {10};

    // Use a nominal memory size (actual value doesn't matter for Windows stub)
    const size_t memSize = 8ULL * 1024 * 1024 * 1024;  // 8GB nominal

    mNumaNodeTbl.emplace_back(0, 1, memSize, cpuIdList, nodeDistance);
}

const NumaNode*
NumaUtil::getNumaNode(const unsigned nodeId) const
{
    if (nodeId >= mNumaNodeTbl.size()) return nullptr;
    return &mNumaNodeTbl[nodeId];
}

const NumaNode*
NumaUtil::findNumaNodeByCpuId(const unsigned cpuId) const
{
    // On Windows, all CPUs belong to node 0
    if (!mNumaNodeTbl.empty()) {
        return &mNumaNodeTbl[0];
    }
    return nullptr;
}

std::vector<unsigned>
NumaUtil::genActiveNumaNodeIdTblByCpuIdTbl(const std::vector<unsigned>& cpuIdTbl) const
{
    // On Windows, there's only one virtual NUMA node
    std::vector<unsigned> result;
    if (!cpuIdTbl.empty() && !mNumaNodeTbl.empty()) {
        result.push_back(0);
    }
    return result;
}

// static function
unsigned
NumaUtil::findNumaNodeByMemAddr(void* addr)
{
    // On Windows without NUMA support, all memory is on node 0
    return 0;
}

std::string
NumaUtil::show() const
{
    std::ostringstream ostr;
    ostr << "NumaUtil (Windows stub - NUMA not supported) { nodes:" << mNumaNodeTbl.size() << " }";
    return ostr.str();
}

} // namespace scene_rdl2

#endif // _WIN32
