////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2011.
// -------------------------------------------------------------------------
//  File name:   IFlowgraphDebugger.h
//  Version:     v1.00
//  Created:     13/09/2011 by Sascha Hoba.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __IFLOWGRAPHDEBUGGER_H__
#define __IFLOWGRAPHDEBUGGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <IFlowSystem.h>
#include <CryExtension/ICryUnknown.h>
#include <CryExtension/CryCreateClassInstance.h>

static const char* IFlowgraphDebuggerExtensionName = "FlowgraphDebugger";

class CFlowNode;
class CHyperNodePort;

struct BreakPointPortInfo
{
		TFlowPortId portID;
		bool bIsEnabled;
		bool bIsTracepoint;

		BreakPointPortInfo():
				portID(InvalidFlowPortId), 
				bIsEnabled(true),
				bIsTracepoint(false)
		{
		}
};

typedef std::vector<BreakPointPortInfo> TFlowPortIDS;

// we support adding breakpoints for outputs too, so we don't have to add extra flownodes
// to see if an output was actually triggered
struct BreakPoints
{
	TFlowPortIDS inputPorts;
	TFlowPortIDS outputPorts;
};

// Stores breakpoints per flownode used for the final debug information
typedef std::map<TFlowNodeId, BreakPoints> TFlowNodesDebugInfo;

// Stores debug information per flowgraph
typedef std::map<IFlowGraphPtr, TFlowNodesDebugInfo> TDebugInfo;

enum EBreakpointType
{
		eBT_Invalid = 0,
		eBT_Input,
		eBT_Output,
		eBT_Output_Without_Edges,
		eBT_Num_Breakoint_Types
};

struct BreakPoint
{
	IFlowGraphPtr flowGraph;
	SFlowAddress addr;
	SFlowAddress addr2;
	TFlowInputData value;
	EBreakpointType type;
	int edgeIndex;

	BreakPoint():flowGraph(NULL), addr(), addr2(), value(), type(eBT_Invalid), edgeIndex(-1){}
	BreakPoint(const BreakPoint& breakpoint):
		flowGraph(breakpoint.flowGraph),
		addr(breakpoint.addr),
		addr2(breakpoint.addr2),
		value(breakpoint.value),
		type(breakpoint.type),
		edgeIndex(breakpoint.edgeIndex)
	{
	}
};

struct IFlowgraphDebugListener
{
	// Called once a new breakpoint was added
	virtual void OnBreakpointAdded(const BreakPoint& breakpoint) = 0;

	// Called once a breakpoint was removed
	virtual void OnBreakpointRemoved(const BreakPoint& breakpoint) = 0;

	// Called once a specific flownode has no breakpoints anymore
	virtual void OnAllBreakpointsRemovedForNode(IFlowGraphPtr pFlowGraph, TFlowNodeId nodeID) = 0;

	// Called once a specific flowgraph has no breakpoints anymore
	virtual void OnAllBreakpointsRemovedForGraph(IFlowGraphPtr pFlowGraph) = 0;

	// Called once a breakpoint was hit
	virtual void OnBreakpointHit(const BreakPoint& breakpoint) = 0;

	virtual void OnTracepointHit(const BreakPoint& tracepoint) = 0;

	virtual void OnBreakpointInvalidated(const BreakPoint& breakpoint) = 0;

	virtual void OnEnableBreakpoint(const BreakPoint& breakpoint, bool enable) = 0;

	virtual void OnEnableTracepoint(const BreakPoint& tracepoint, bool enable) = 0;

protected:
		virtual ~IFlowgraphDebugListener() {}; 
};

struct IFlowgraphDebugger : public ICryUnknown
{
		CRYINTERFACE_DECLARE( IFlowgraphDebugger, 0x416CE2E1B23B4017, 0xAD93D04DA67E90E6 )

public:
	// Adds a new breakpoint for a specific flownode and port
	virtual bool AddBreakpoint(IFlowGraphPtr pFlowgraph, const SFlowAddress& addr) = 0;

	// Removes a breakpoint for a specific flownode at a specific port
	virtual bool RemoveBreakpoint(IFlowGraphPtr pFlowgraph, const SFlowAddress& addr) = 0;
	
	// Removes ALL breakpoints for a specific flownode
	virtual bool RemoveAllBreakpointsForNode(IFlowGraphPtr pFlowGraph, TFlowNodeId nodeID) = 0;

	// Removes ALL breakpoints for a specific flowgraph
	virtual bool RemoveAllBreakpointsForGraph(IFlowGraphPtr pFlowgraph) = 0;

	// Checks if a specific flownode with a specific port has a breakpoint
	virtual bool HasBreakpoint(IFlowGraphPtr pFlowGraph, const SFlowAddress& addr) const = 0;

	// Checks if a flownode in a specific flowgraph has at least one breakpoint
	virtual bool HasBreakpoint(IFlowGraphPtr pFlowGraph, TFlowNodeId nodeID) const = 0;

	// Checks if a flowgraph has at least one breakpoint
	virtual bool HasBreakpoint(IFlowGraphPtr pFlowGraph) const = 0;

	virtual bool EnableBreakpoint(IFlowGraphPtr pFlowgraph, const SFlowAddress& addr, bool enable) = 0;

	virtual bool EnableTracepoint(IFlowGraphPtr pFlowgraph, const SFlowAddress& addr, bool enable) = 0;

	virtual bool IsBreakpointEnabled(IFlowGraphPtr pFlowgraph, const SFlowAddress& addr) = 0;

	virtual bool IsTracepoint(IFlowGraphPtr pFlowgraph, const SFlowAddress& addr) = 0;

	// Returns information about the current breakpoint
	virtual const BreakPoint& GetBreakpoint() = 0;

	// Invalidates the SFlowAdress and the flowgraph pointer of the stored breakpoint
	virtual void InvalidateBreakpoint() = 0;

	// Returns true if a breakpoint was hit, false otherwise
	virtual bool BreakpointHit() = 0;

	// Adds a new listener who is interested to listen to certain debug actions like adding a new breakpoint etc.
	virtual bool RegisterListener(IFlowgraphDebugListener* pListener, const char* name) = 0;

	// Removes a listener who is not interested in any debugger events anymore
	virtual void UnregisterListener(IFlowgraphDebugListener* pListener) = 0;

	// Perform Activation of a port
	virtual bool PerformActivation(IFlowGraphPtr pFlowgraph, const SFlowAddress& addr, const TFlowInputData& value) = 0;

	// Perform Activation of a port
	virtual bool PerformActivation(IFlowGraphPtr pFlowgraph, int edgeIndex, const SFlowAddress& fromAddr, const SFlowAddress& toAddr, const TFlowInputData& value) = 0;

	virtual bool RePerformActivation() = 0;

	// Removes all internally stored breakpoints
	virtual void ClearBreakpoints() = 0;

	virtual bool GetBreakpoints(DynArray<BreakPoint>& breakpointsDynArray) = 0;

};

DECLARE_BOOST_POINTERS( IFlowgraphDebugger );

static IFlowgraphDebuggerPtr GetIFlowgraphDebuggerPtr()
{
		IFlowgraphDebuggerPtr pFlowgraphDebugger;
		CryCreateClassInstance(IFlowgraphDebuggerExtensionName, pFlowgraphDebugger);
		return pFlowgraphDebugger;
}

#endif
