///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (c) 2019, Nefelus Inc
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "extSpef.h"
//#include "dbExtControl.h"
#include <dbExtControl.h>
#include "extRCap.h"
#include "parse.h"
#include <math.h>
#include <algorithm>
//#include "logger.h"
#include <logger.h>

//#ifdef _WIN32
#define ATH__fprintf fprintf
#define ATH__fopen fopen
#define ATH__fclose fclose
//#endif

BEGIN_NAMESPACE_ADS

uint extSpef::writeHierInstNameMap()
{
    dbSet<dbInst> insts = _block->getInsts();
    dbSet<dbInst>::iterator itr;

	uint instCnt= 0;
    for( itr = insts.begin(); itr != insts.end(); ++itr )
    {
        dbInst * inst = *itr;
        dbBlock * child = inst->getChild();

        if ( child==NULL )
		continue;

	dbIntProperty::create(child, "_instSpefMapBase", _baseNameMap);
	int baseMapId= extSpef::getIntProperty(child, "_instSpefMapBase");
	debug("HEXT", "S", "\ninstMapBase %d : %s  %s\n", 
		baseMapId, child->getConstName(), inst->getConstName());

	uint mapId= 0;
    	dbSet<dbInst> binsts = child->getInsts();
    	dbSet<dbInst>::iterator bitr;
    	for( bitr = binsts.begin(); bitr != binsts.end(); ++bitr )
    	{
        	dbInst * ii = *bitr;

		mapId= _baseNameMap+ii->getId();

		char *nname = (char *) ii->getConstName();
                char *nname1 = tinkerSpefName(nname); 
		ATH__fprintf(_outFP, "*%d %s/%s\n", mapId, inst->getConstName(), nname1);
		debug("HEXT", "S", "\t%d %s/%s\n",
			mapId, inst->getConstName(), nname1);

		instCnt++;
	}
	_baseNameMap= mapId;
	debug("HEXT", "S", "END_MAP %d inames : %s\n\n", 
		instCnt, child->getConstName());
    }
	return instCnt;
}
int extSpef::getIntProperty(dbBlock *block, const char *name)
{
        dbProperty *p= dbProperty::find(block, name);

        if (p==NULL)
                return 0;

        dbIntProperty *ip= (dbIntProperty*) p;
	return ip->getValue();
}

uint extSpef::writeHierNetNameMap()
{
    dbSet<dbInst> insts = _block->getInsts();
    dbSet<dbInst>::iterator itr;

	uint netCnt= 0;
    for( itr = insts.begin(); itr != insts.end(); ++itr )
    {
        dbInst * inst = *itr;
        dbBlock * child = inst->getChild();

        if ( child==NULL )
		continue;

	dbIntProperty::create(child, "_netSpefMapBase", _baseNameMap);
	int baseMapId= extSpef::getIntProperty(child, "_netSpefMapBase");
	
	debug("HEXT", "S", "\nnetMapBase %d : %s  %s\n", 
		baseMapId, child->getConstName(), inst->getConstName());

	uint mapId= 0;
    	dbSet<dbNet> nets = child->getNets();
    	dbSet<dbNet>::iterator bitr;
    	for( bitr = nets.begin(); bitr != nets.end(); ++bitr )
    	{
        	dbNet * ii = *bitr;

		if (ii->getBTermCount()>0)
			continue;

		mapId= _baseNameMap+ii->getId();

		char *nname = (char *)ii->getConstName();
                char *nname1 = tinkerSpefName(nname); 
		ATH__fprintf(_outFP, "*%d %s/%s\n", mapId, inst->getConstName(), nname1);
		debug("HEXT", "S", "\t%d %s/%s\n",
			mapId, inst->getConstName(), nname1);
		netCnt ++;
	}
	_baseNameMap= mapId;
	debug("HEXT", "S", "NET_MAP_END (%d internal nets) : %s\n\n",
		netCnt, child->getConstName());

    }
	// OPTIMIZE this ; have to do due to 1st "null" rseg; 
	// if not, appending rsegs creates a havoc!

    	dbSet<dbNet> topnets = _block->getNets();
    	dbSet<dbNet>::iterator bitr;
    	for( bitr = topnets.begin(); bitr != topnets.end(); ++bitr )
    	{
        	dbNet * ii = *bitr;

		//extMain::printRSegs(ii);
        	dbSet<dbRSeg> rSet= ii->getRSegs();
        	rSet.reverse();
		//extMain::printRSegs(ii);
	}
	return netCnt;
}
uint extMain::markCCsegs(dbBlock *blk, bool flag)
{
	dbSet<dbCCSeg> ccs= blk->getCCSegs();
    	dbSet<dbCCSeg>::iterator ccitr;
	for (ccitr= ccs.begin(); ccitr != ccs.end(); ++ccitr) {
		dbCCSeg *cc= *ccitr;
		cc->setMark(flag);
	}
	return ccs.size();
}

uint extMain::addRCtoTop(dbBlock *blk, bool write_spef)
{
	notice(0, "\nMerging Parasitics for Block %s : %s Into parent %s\n",
		blk->getConstName(), 
		blk->getParentInst()->getConstName(), _block->getConstName());

		//blk->getParent()->getConstName()

	int netBaseMapId= extSpef::getIntProperty(blk, "_netSpefMapBase");
	int instBaseMapId= extSpef::getIntProperty(blk, "_instSpefMapBase");
	_spef->setHierBaseNameMap(instBaseMapId, netBaseMapId);

	debug("HEXT", "F", "\n_netSpefMapBase= %d _instSpefMapBase= %d\n",
			netBaseMapId, instBaseMapId);

	dbSet<dbCapNode> allcapnodes= blk->getCapNodes();
	uint blockCapSize= allcapnodes.size();

	uint *capNodeMap= new uint[2*blockCapSize+1];

	bool foreign= false;
	uint spefCnt= 0;
	uint flatCnt= 0;
	uint gCnt= 0;
	uint rCnt= 0;
	uint ccCnt= 0;
	double resBound= 0.0;
	uint dbg= 0;

	dbNet *topDummyNodeNet= _block->findNet("dummy_sub_block_cap_nodes");
	if (topDummyNodeNet==NULL)
		topDummyNodeNet= dbNet::create(_block, "dummy_sub_block_cap_nodes");

    	dbSet<dbNet> nets = blk->getNets();
    	dbSet<dbNet>::iterator bitr;
    	for( bitr = nets.begin(); bitr != nets.end(); ++bitr )
    	{
        	dbNet * net = *bitr;

		if (!net->setIOflag())
			continue;

		dbBTerm *bterm= net->get1stBTerm();
            	dbITerm * iterm = bterm->getITerm();
            	dbNet * parentNet = iterm->getNet();
		if (parentNet==NULL) {
			warning(0, "Null parent[%d] net : %s\n", 
				net->getBTermCount(), net->getConstName());
			continue;
		}

		gCnt += createCapNodes(net, parentNet, capNodeMap, instBaseMapId);
		rCnt += createRSegs(net, parentNet, capNodeMap);
		flatCnt ++;
	}
	markCCsegs(blk, false);
    	for( bitr = nets.begin(); bitr != nets.end(); ++bitr )
    	{
        	dbNet * net = *bitr;
		if (!net->isIO())
			continue;

		dbBTerm *bterm= net->get1stBTerm();
            	dbITerm * iterm = bterm->getITerm();
            	dbNet * parentNet = iterm->getNet();
		if (parentNet==NULL) {
			warning(0, "Null parent[%d] net : %s\n", 
				net->getBTermCount(), net->getConstName());
			continue;
		}

		ccCnt += createCCsegs(net, parentNet, topDummyNodeNet, capNodeMap,
				instBaseMapId);
	}
	markCCsegs(blk, false);

	_spef->setBlock(blk); 
    	for( bitr = nets.begin(); bitr != nets.end(); ++bitr )
    	{
        	dbNet * net = *bitr;
		if (net->isIO())
			continue;

		//extMain::printRSegs(net);
		if (write_spef)
			spefCnt += _spef->writeHierNet(net, resBound, dbg);
	}
	_spef->setBlock(_block);
	notice(0, "%d internal %d IO nets %d gCaps %d rSegs %d ccCaps : %s\n\n",
			spefCnt, flatCnt, gCnt, rCnt, ccCnt, blk->getConstName());

	delete [] capNodeMap;
	return flatCnt+spefCnt;
}
uint extMain::adjustParentNode(dbNet *net, dbITerm *from_child_iterm, uint node_num)
{
	dbSet<dbCapNode> capNodes= net->getCapNodes();
    	dbSet<dbCapNode>::iterator cap_node_itr= capNodes.begin();
    	for( ; cap_node_itr != capNodes.end(); ++cap_node_itr ) {
		dbCapNode *node= *cap_node_itr;
		if (!node->isITerm())
			continue;
		dbITerm *iterm= node->getITerm();
		if (iterm!=from_child_iterm)
			continue;

		node->resetITermFlag();
		node->setInternalFlag();

		node->setNode(node_num);

		return node->getId();
	}
	return 0;
}

bool extMain::createParentCapNode(dbCapNode *node, dbNet *parentNet, uint nodeNum, uint *capNodeMap, uint baseNum)
{
	bool foreign= false;
	char buf[1024];

       	dbCapNode *cap= NULL;
	if (!node->isBTerm()) { // 
       		cap= dbCapNode::create(parentNet, nodeNum, foreign);
		capNodeMap[node->getId()]= cap->getId();
	}
	if (node->isInternal()) { // 
       		cap->setInternalFlag();

		debug("HEXT","C", "\t\tG intrn %d --> %d\n", 
			node->getId(), capNodeMap[node->getId()]);
	}
	else if (node->isITerm()) { // 
		dbITerm *iterm= node->getITerm();
		dbInst *inst= iterm->getInst();
		uint instId= inst->getId();
		sprintf(buf, "*%d%s%s", baseNum+instId, 
			_spef->getDelimeter(),
			iterm->getMTerm()->getConstName());

		dbStringProperty::create(cap, "_inode", buf);
       		cap->setNode(baseNum+instId); // so there is some value
       		cap->setNameFlag();

		debug("HEXT","C", "\t\tG ITerm %d --> %d : %s\n", 
			node->getId(), capNodeMap[node->getId()], buf);
	}
	else if (node->isBTerm()) { // 

		dbBTerm *bterm= node->getBTerm();
       		dbITerm * iterm = bterm->getITerm();
		uint parentId= adjustParentNode(parentNet, iterm, nodeNum);
		capNodeMap[node->getId()]= parentId;

		debug("HEXT","C", "\t\tG BTerm %d --> %d : %d\n", 
			node->getId(), parentId, nodeNum);
	}
	return true;
}
void extMain::createTop1stRseg(dbNet *net, dbNet *parentNet)
{
	if (parentNet->getWire()!=NULL)
		return;
	
	dbBTerm *bterm= net->get1stBTerm();
	dbITerm *iterm = bterm->getITerm();

	dbCapNode *cap= dbCapNode::create(parentNet, 0, /*_foreign*/ false); 
	cap->setNode(iterm->getId()); 
	cap->setITermFlag();
	dbRSeg *rc= dbRSeg::create(parentNet, 0, 0, 0, true);
	rc->setTargetNode(cap->getId());
}
uint extMain::createCapNodes(dbNet *net, dbNet *parentNet, uint *capNodeMap, uint baseNum)
{
	createTop1stRseg(net, parentNet);

	// have to improve for performance
	uint maxCap= parentNet->maxInternalCapNum()+1;
	debug("HEXT", "C", "\n\tCapNodes: maxCap=%d : %s %s\n", 
		maxCap, net->getConstName(), parentNet->getConstName());

	uint gCnt= 0;
	bool foreign= false;
	char buf[1024];
	dbSet<dbCapNode> capNodes= net->getCapNodes();
    	dbSet<dbCapNode>::iterator cap_node_itr= capNodes.begin();
    	for( ; cap_node_itr != capNodes.end(); ++cap_node_itr ) {
		dbCapNode *node= *cap_node_itr;

		uint nodeNum= maxCap++;

		gCnt += createParentCapNode(node, parentNet, nodeNum, capNodeMap, baseNum);
	}
	return gCnt;
}
uint extMain::printRSegs(dbNet *net)
{
	notice(0, "\t\t\tprintRSegs: %s\n", net->getConstName());
	dbSet<dbRSeg> rsegs= net->getRSegs();
	uint rsize= rsegs.size();

	uint rCnt= 0;
    	dbSet<dbRSeg>::iterator rseg_itr= rsegs.begin();
    	for( ; rseg_itr != rsegs.end(); ++rseg_itr ) {
		dbRSeg *rseg= *rseg_itr;

		notice(0, "\t\t\t\t\trsegId: %d -- %d %d\n", rseg->getId(),
			rseg->getSourceNode(), rseg->getTargetNode());
		rCnt ++;
	}
	return rCnt;
}
uint extMain::createRSegs(dbNet *net, dbNet *parentNet, uint *capNodeMap)
{
	debug("HEXT", "R", "\n\tRSegs: %s %s\n", 
		net->getConstName(), parentNet->getConstName());

	//extMain::printRSegs(parentNet);

	dbSet<dbRSeg> rsegs= net->getRSegs();
	uint rsize= rsegs.size();

	uint rCnt= 0;
    	dbSet<dbRSeg>::iterator rseg_itr= rsegs.begin();
    	for( ; rseg_itr != rsegs.end(); ++rseg_itr ) {
		dbRSeg *rseg= *rseg_itr;

		int x, y;
		rseg->getCoords(x, y);
		uint pathDir= rseg->pathLowToHigh() ? 0 : 1;
		dbRSeg *rc= dbRSeg::create(parentNet, x, y, pathDir, true);

		uint tgtId= rseg->getTargetNode();
		uint srcId= rseg->getSourceNode();

		rc->setSourceNode(capNodeMap[srcId]);
		rc->setTargetNode(capNodeMap[tgtId]);

		for (uint corner= 0; corner<_block->getCornerCount(); corner++) {
			double res= rseg->getResistance(corner);
			double cap= rseg->getCapacitance(corner);

			rc->setResistance(res, corner);
			rc->setCapacitance(cap, corner);
			debug("HEXT", "R","\t\tsrc:%d->%d - tgt:%d->%d - %g  %g\n",
				srcId, capNodeMap[srcId], tgtId, capNodeMap[tgtId],
				res, cap);
		}
		rCnt ++;
	}
	//extMain::printRSegs(parentNet);
	/*
	if (rCnt>0) {
        	dbSet<dbRSeg> rSet= parentNet->getRSegs();
        	//rSet.reverse();
	}
	*/
	return rCnt;
}
bool extMain::adjustChildNode(dbCapNode *childNode, dbNet *parentNet, uint *capNodeMap)
{
	char buf[1024];
	uint parentId= capNodeMap[childNode->getId()];
	dbCapNode *parentNode= dbCapNode::getCapNode(_block, parentId);

	if (childNode->isInternal() || childNode->isBTerm()) { // 
		childNode->resetInternalFlag();
		childNode->resetBTermFlag();

		// spefParentNameId == parent netId (just lucky!!)
		sprintf(buf, "*%d%s%d", parentNet->getId(),
				_spef->getDelimeter(), parentNode->getNode());

		dbStringProperty::create(childNode, "_inode", buf);

		debug("HEXT","CC", "\t\tCC intNode %d --> %d : %s\n", 
				childNode->getId(), parentId, buf);
	}
	else if (childNode->isITerm()) { // 
		childNode->resetITermFlag();

		dbStringProperty *p= dbStringProperty::find(parentNode, "_inode");
		dbStringProperty::create(childNode,"_inode", p->getValue().c_str());

		debug("HEXT","CC", "\t\tCC ITermNode %d --> %d : %s\n", 
			childNode->getId(), parentId, p->getValue().c_str());
	}
        childNode->setNameFlag();
}
uint extMain::createCCsegs(dbNet *net, dbNet *parentNet, dbNet *topDummyNet, uint *capNodeMap, uint baseNum)
{
	//IO nets

	// have to improve for performance
	uint maxCap= parentNet->maxInternalCapNum()+1;
	
	debug("HEXT", "CC", "\tCCsegs: maxCap[%d] %s %s\n", maxCap,
		net->getConstName(), parentNet->getConstName());

	dbBlock *pblock= parentNet->getBlock();
	uint ccCnt= 0;

	dbSet<dbCapNode> nodeSet = net->getCapNodes();
	dbSet<dbCapNode>::iterator rc_itr;
	for( rc_itr = nodeSet.begin(); rc_itr != nodeSet.end(); ++rc_itr ) {
        	dbCapNode *srcCapNode = *rc_itr; 

		dbSet<dbCCSeg> ccsegs= srcCapNode->getCCSegs();
    		dbSet<dbCCSeg>::iterator ccseg_itr= ccsegs.begin();
    		for( ; ccseg_itr != ccsegs.end(); ++ccseg_itr ) {
			dbCCSeg *cc= *ccseg_itr; 

			if (cc->isMarked())
				continue;

			dbCapNode *dstCapNode= cc->getTargetCapNode();
                        if (cc->getSourceCapNode()!=srcCapNode) { 
				dstCapNode= cc->getSourceCapNode();
			}
			if (!dstCapNode->getNet()->isIO()) {
				uint nodeNum= maxCap++;
				createParentCapNode(dstCapNode, topDummyNet, nodeNum, capNodeMap, baseNum);
				adjustChildNode(srcCapNode, parentNet, capNodeMap);
			}
			uint tId= dstCapNode->getId();
			uint sId= srcCapNode->getId();
			uint tgtId= capNodeMap[tId];
			uint srcId= capNodeMap[sId];

			dbCapNode *map_tgt= dbCapNode::getCapNode(pblock, tgtId);
			dbCapNode *map_src= dbCapNode::getCapNode(pblock, srcId);

			dbCCSeg *ccap= dbCCSeg::create(map_src, map_tgt, false);

			for (uint corner= 0; corner<_block->getCornerCount(); corner++) {
				double cap= cc->getCapacitance(corner);
				ccap->setCapacitance(cap, corner);

				debug("HEXT", "CC","\t\tCC src:%d->%d tgt:%d->%d CC %g\n",
					sId, srcId, tId, tgtId, cap);
			}
			cc->setMark(true);
		}
	}
	return maxCap;
}
/*
uint extMain::adjustCCsegs(dbNet *net, uint baseNum)
{
	//Internal nets

	// have to improve for performance
	uint maxCap= parentNet->maxInternalCapNum()+1;
	
	debug("HEXT", "F", "\tCCsegs: maxCap[%d] %s %s\n", maxCap,
		net->getConstName(), parentNet->getConstName());

	dbBlock *pblock= parentNet->getBlock();
	uint ccCnt= 0;

	dbSet<dbCapNode> nodeSet = net->getCapNodes();
	dbSet<dbCapNode>::iterator rc_itr;
	for( rc_itr = nodeSet.begin(); rc_itr != nodeSet.end(); ++rc_itr ) {
        	dbCapNode *srcCapNode = *rc_itr; 

		dbSet<dbCCSeg> ccsegs= srcCapNode->getCCSegs();
    		dbSet<dbCCSeg>::iterator ccseg_itr= ccsegs.begin();
    		for( ; ccseg_itr != ccsegs.end(); ++ccseg_itr ) {
			dbCCSeg *cc= *ccseg_itr; 

			if (cc->isMarked())
				continue;

				adjustChildNode(srcCapNode, parentNet, capNodeMap);
			cc->setMark(true);
		}
	}
	return maxCap;
}
*/
void extSpef::writeDnetHier(uint mapId, double *totCap)
{
        if (_writeNameMap)
                ATH__fprintf(_outFP, "\n*D_NET *%d ",mapId);
        //else
                //ATH__fprintf(_outFP, "\n*D_NET %s ", tinkerSpefName((char *)_d_net->getConstName()));
        writeRCvalue(totCap, _cap_unit);
        ATH__fprintf(_outFP, "\n");
}

bool extSpef::writeHierNet(dbNet *net, double resBound, uint dbg)
{
	_block= net->getBlock();
	_d_net = net;
	uint netId= net->getId();
	_cornerBlock= net->getBlock();

	_cornersPerBlock = _cornerCnt;

	//if (_cornerBlock && _cornerBlock!=_block)
		//net= dbNet::getNet(_cornerBlock, netId);

	uint minNode;
	uint capNodeCnt= getMinCapNode(net, &minNode);
	if (capNodeCnt==0) {
		warning(0,"No cap nodes net: %s\n", net->getConstName());
		return false;
	}
	dbSet<dbRSeg> rcSet= net->getRSegs();

	_cCnt= 1;
	// must set at calling _symmetricCCcaps= true; //TODO non symmetric case??
	// must set at calling _preserveCapValues= false // true only for reading in SPEF

	double totCap[5];
	resetCap(totCap);
	if (_symmetricCCcaps)
		addCouplingCaps(net, totCap);

	_firstCapNode= minNode-1;

	reinitCapTable(_nodeCapTable, capNodeCnt+2);

	if (_singleP)
		computeCapsAdd2Target(rcSet, totCap);
	else
		computeCaps(rcSet, totCap);

	debug("HEXT", "S", "\tDNET *%d-%d %g %s\n", _childBlockNetBaseMap+netId, 
		netId, totCap[0], net->getConstName());

	//netId= _childBlockNetBaseMap+netId;
	writeDnetHier(_childBlockNetBaseMap+netId, totCap);

	if (_wConn) {
		writeKeyword("*CONN");
		writePorts(net);
		writeITerms(net);
	}
	if (_writingNodeCoords == C_STARRC)
		writeNodeCoords(netId, rcSet);

	if (_wCap || _wOnlyCCcap)
		writeKeyword("*CAP");

	if (_wCap && !_wOnlyCCcap) {
		writeCapPorts(net);
		writeCapITerms(net);
		writeNodeCaps(net, _childBlockNetBaseMap+netId);
	}

	if (_wCap || _wOnlyCCcap)
		writeSrcCouplingCaps(net);

	if (_symmetricCCcaps && (_wCap || _wOnlyCCcap))
		writeTgtCouplingCaps(net);

	if (_wRes) {
		writeRes(netId, rcSet);
	}
	writeKeyword("*END");

        dbSet<dbCapNode> nodeSet= net->getCapNodes();
        dbSet<dbCapNode>::iterator rc_itr;
        for( rc_itr = nodeSet.begin(); rc_itr != nodeSet.end(); ++rc_itr ) {
                dbCapNode* node = *rc_itr;
                node->setSortIndex(0);
        }
        return true;
}
void extSpef::setHierBaseNameMap(uint instBase, uint netBase)
{
	_childBlockNetBaseMap= netBase;
	_childBlockInstBaseMap= instBase;
}

END_NAMESPACE_ADS

