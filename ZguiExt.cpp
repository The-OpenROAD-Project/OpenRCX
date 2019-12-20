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
#include "ZguiExt.h"
//#include "ZImplements.hpp"
//#include "ZFactory.h"
#include <ZImplements.hpp>
#include <ZFactory.h>
#include "extRCap.h"
//#include "logger.h"
#include <logger.h>

DECLARE_ZFACTORY(ZguiExt, IZgui)


BEGIN_NAMESPACE_ADS


DECLARE_IMPLEMENTATION(ZguiExt, IZgui)


ZguiExt::ZguiExt()
{
	_dcr= NULL;
	_ext= NULL;
}
ZguiExt::~ZguiExt()
{
}
void ZguiExt::setGuiContext(void *x, void *blk)
{
	_ext= (extMain *) x;
}
void ZguiExt::setDcr(ZPtr<IZdcr> z)
{
	_dcr= z;
}
char* ZguiExt::makeName(const char *name)
{	
	uint len= strlen(name);
	char *a= new char[len+1];
	strcpy(a, name);
	return a;
}
char *ZguiExt::setName(const char *name)
{	
	_name= makeName(name);
	return _name;
}
char *ZguiExt::getName()
{	
	return _name;
}
void ZguiExt::init(const char *name)
{
	//ZALLOCATED(_zui);

	_name= makeName(name);
}
uint ZguiExt::getBbox(int *x1, int *y1, int *x2, int *y2)
{
	return _ext->getExtBbox(x1, y1, x2, y2);
}

void ZguiExt::chip_get(ZPtr<IZdcr> dcr)
{
/*
	if (! dcr->isBlockSelected(_ext->getBlock()->getId()))
		return;

	notice(0, "chip_get from Extractor\n");

	_ext->getExtBoxes(dcr, 1); // 1 is 
*/
}
void ZguiExt::inspect(ZPtr<IZdcr> dcr)
{
	_dcr= dcr;

	if (! dcr->isBlockSelected(_ext->getBlock()->getId()))
		return;

	if (_dcr->selectAction())
	{
		if (!_dcr->isSelectedMenu("rctree"))
			return;


		uint netId= _dcr->getSubmenuObjId(NULL);
//		dbBlock *blk= _ext->getBlock();

		_ext->makeTree(netId);

		return;
	}

	//notice(0, "inspect from Extractor\n");
	
	_ext->extInspect(dcr, 1);
/*
	if (zui->selectAction())
	{
		getNetConnectivity(zui, getDbNet(zui), false, true, true);
		return;
	}
	uint wid= zui->getInspNameId();

	Ath__searchBox bb;
	char* wireVia= "Wire";
	dbNet *net= getNetAndCoords(&bb, true, true, wid);
	if (net==NULL) 
	{
		net= getNetAndCoords(&bb, false, true, wid);
		wireVia= "Via";
	}

	zui->setCurrentMark(Ath_mark__main);
	
	if (zui->msgAction())
	{
		char msg_buf[1024];
		sprintf(0sg_buf, "Signal %s %d %s Block %s",
			wireVia, net->getId(), net->getName().c_str(),
			_block->getName().c_str());
		
		zui->print_wire_msg(bb.loXY(0), bb.loXY(1), bb.hiXY(0), bb.hiXY(1), 
			bb.getLevel(), msg_buf);
	}
	else if (zui->selfAction()) {
		
		zui->addBox(&bb, Ath_hier__signal, Ath_box__wire, wid);

		Ath__searchBox maxBB(&bb,0);
		uint len= getNetMaxBox(net, &maxBB);
		char buf[100];
		sprintf(buf, "wireLength %d", len);

		dbString name = net->getName();
		
        dbString designName = _block->getName();
		zui->print_self((char *) designName.c_str(), (char *) name.c_str(), buf);
	}
	else if (zui->groupAction1()) 
	{
		getNetConnectivity(zui, net, false, false, false);
	}
	else if (zui->groupAction2()) 
	{
		getNetConnectivity(zui, net, true, false, false);
	}
	else if (zui->linkAction()) {
		//getNetConnectivity(zui, net);
	}
*/
}

END_NAMESPACE_ADS
