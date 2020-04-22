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
#include <errno.h>
#include "ext.h"
#include "../dcr/Crawler.h"
#include "ext_Tcl.h"
//#include "logger.h"
#include <logger.h>

#define TCL_IN_ARGS(name) name * in_args = (name *) in

#define TCL_OUT_ARGS(name) name * out_args = (name *) out

BEGIN_NAMESPACE_ADS

Ext::Ext( ZArgs * in, ZArgs * out ) : ZTechModule<Ext>( in, out )
{
//	ZIn_Ext_Ext * in_args = (ZIn_Ext_Ext *) in;
	_ext= new extMain(5);
	_ext->setDB(_db);
	_tree= NULL;
}
Ext::~Ext()
{
	delete _ext;
}
TCL_METHOD ( Ext::load_model )
{
    TCL_IN_ARGS ( ZIn_Ext_load_model );

	int setMin= in_args->set_min();
	int setMax= in_args->set_max();
	int setTyp= in_args->set_typ();

	if (in_args->lef_rc()) {
		if (!_ext->checkLayerResistance())
			return TCL_ERROR;
		_ext->addExtModel();	//fprintf(stdout, "Using LEF RC values to extract!\n");
		notice(0, "Using LEF RC values to extract!\n");
	}
	else if (in_args->file()!=NULL) {
		_ext->readExtRules(in_args->name(), in_args->file(), setMin, setTyp, setMax);
		int numOfNet, numOfRSeg, numOfCapNode, numOfCCSeg;
		_ext->getBlock()->getExtCount(numOfNet, numOfRSeg, numOfCapNode, numOfCCSeg);

		_ext->setupMapping(3*numOfNet);
	}
	else {
		//fprintf(stdout, "\nHave to specify options:\n\t-lef_rc to read resistance and capacitance values from LEF or \n\t-file to read high accuracy RC models\n");
		notice(0, "\nHave to specify options:\n\t-lef_rc to read resistance and capacitance values from LEF or \n\t-file to read high accuracy RC models\n");
	}

    return 0;
}
TCL_METHOD ( Ext::read_process )
{
    TCL_IN_ARGS ( ZIn_Ext_read_process );

	_ext->readProcess(in_args->name(), in_args->file());

    return TCL_OK;
}
TCL_METHOD ( Ext::rules_gen )
{
    TCL_IN_ARGS ( ZIn_Ext_rules_gen );

	_ext->rulesGen(in_args->name(), in_args->dir(), in_args->file(), in_args->pattern(),
		in_args->write_to_solver(), in_args->read_from_solver(), in_args->run_solver(), in_args->keep_file());

    return TCL_OK;
}
TCL_METHOD ( Ext::metal_rules_gen )
{
    TCL_IN_ARGS ( ZIn_Ext_metal_rules_gen );
                                                                                
	_ext->metRulesGen(in_args->name(), in_args->dir(), in_args->file(), in_args->pattern(),
		in_args->write_to_solver(), in_args->read_from_solver(), in_args->run_solver(), in_args->keep_file(), in_args->metal());
	return TCL_OK;
}
TCL_METHOD ( Ext::write_rules )
{
    TCL_IN_ARGS ( ZIn_Ext_write_rules );
	_ext->writeRules(in_args->name(), in_args->dir(), in_args->file(), in_args->pattern(), in_args->read_from_solver(), in_args->read_from_db());
	return TCL_OK;
}
TCL_METHOD ( Ext::get_ext_metal_count)
{
    TCL_OUT_ARGS ( ZOut_Ext_get_ext_metal_count );

	extRCModel *m= _ext->getRCModel();
	int cnt = m->getLayerCnt();
	out_args->metal_count(cnt);
	return TCL_OK;
}
TCL_METHOD ( Ext::bench_net )
{
    TCL_IN_ARGS ( ZIn_Ext_bench_net );

	extMainOptions opt;

	opt._write_to_solver= in_args->write_to_solver();
	opt._read_from_solver= in_args->read_from_solver();
	opt._run_solver= in_args->run_solver();

	int netId= in_args->net();
	int trackCnt= in_args->max_track_count();
 	opt._topDir= in_args->dir();

	if (netId==0) {
		notice(0, "Net (=%d), should be a positive number\n", netId);
		return TCL_OK;
	}
    notice(0, "Benchmarking using 3d field solver net %d...\n", netId);
#ifdef ZUI
	ZPtr<ISdb> dbNetSdb= _ext->getBlock()->getNetSdb(_context,  _ext->getTech());

	_ext->benchNets(&opt, netId, trackCnt, dbNetSdb);
#endif
    notice(0, "Finished.\n");

	return TCL_OK;
}
TCL_METHOD ( Ext::run_solver )
{
    TCL_IN_ARGS ( ZIn_Ext_run_solver );
	extMainOptions opt;
	opt._topDir= in_args->dir();
	uint netId= in_args->net();
	int shapeId= in_args->shape();
	_ext->runSolver(&opt, netId, shapeId);
	return TCL_OK;

}
TCL_METHOD ( Ext::bench_wires )
{
    TCL_IN_ARGS ( ZIn_Ext_bench_wires );

	extMainOptions opt;

 	opt._topDir= in_args->dir();
	opt._met_cnt= in_args->met_cnt();
	opt._met= in_args->met();
	opt._overDist= in_args->over_dist();
	opt._underDist= in_args->under_dist();
	opt._overMet= in_args->over_met();
	opt._over= in_args->Over();
	opt._underMet= in_args->under_met();
	opt._len= 1000 * in_args->len();
	opt._wireCnt= in_args->cnt();
	opt._name= in_args->block();

    opt._default_lef_rules= in_args->default_lef_rules();
    opt._nondefault_lef_rules= in_args->nondefault_lef_rules();

	opt._3dFlag= in_args->ddd();
	opt._multiple_widths= in_args->multiple_widths();

	opt._write_to_solver= in_args->write_to_solver();
	opt._read_from_solver= in_args->read_from_solver();
	opt._run_solver= in_args->run_solver();
	opt._diag=in_args->diag();
	opt._db_only=in_args->db_only();

	Ath__parser parser;

	if (in_args->th_list()!=NULL) {	
		parser.mkWords(in_args->th_list());	
		parser.getDoubleArray(&opt._thicknessTable, 0);
		opt._thListFlag= true;
	}
	else {
		parser.mkWords(in_args->th());	
		parser.getDoubleArray(&opt._thicknessTable, 0);
		opt._thListFlag= false;
	}
	opt._listsFlag= false;
	opt._wsListFlag= false;
	if (opt._default_lef_rules) { // minWidth, minSpacing, minThickness, pitch multiplied by grid_list
		// user sets default_flag and grid_list multipliers
		if (in_args->grid_list()==NULL) {
			opt._gridTable.add(1.0);
		}
		else {
			parser.mkWords(in_args->grid_list());	
			parser.getDoubleArray(&opt._gridTable, 0);
		}	
		opt._listsFlag= true;
	}
	else if (opt._nondefault_lef_rules) {
		opt._listsFlag= true;
	}
	else if ((in_args->w_list()!=NULL)&&(in_args->s_list()!=NULL)) {
		opt._listsFlag= true;
		opt._wsListFlag= true;
		parser.mkWords(in_args->w_list());	
		parser.getDoubleArray(&opt._widthTable, 0);
		parser.mkWords(in_args->s_list());	
		parser.getDoubleArray(&opt._spaceTable, 0);
	}
	else {
		parser.mkWords(in_args->w());	
		parser.getDoubleArray(&opt._widthTable, 0);
		parser.mkWords(in_args->s());	
		parser.getDoubleArray(&opt._spaceTable, 0);
		parser.mkWords(in_args->th());	
		parser.getDoubleArray(&opt._thicknessTable, 0);
		parser.mkWords(in_args->d());	
		parser.getDoubleArray(&opt._densityTable, 0);
	}
	_ext->benchWires(&opt);
	
	return TCL_OK;
}
TCL_METHOD ( Ext::bench_verilog_out )
{
	TCL_IN_ARGS(ZIn_Ext_bench_verilog_out);

	char* filename = (char*)in_args->file();
	if (!filename || !filename[0])
	{
		notice(0, "Please set file name.\n");
		return 0;
	}
	FILE* fp = fopen(filename, "w");
	if (fp == NULL) {
		notice(0, "Cannot open file %s\n", filename);
		return 0;
	}
	notice(0, "Writting pattern Verilog %s\n", filename);	
	_ext->benchVerilog(fp);
	return TCL_OK;
}
TCL_METHOD ( Ext::clean )
{
   // TCL_IN_ARGS ( ZIn_Ext_clean );
    return TCL_OK;
}
TCL_METHOD ( Ext::define_process_corner )
{
    TCL_IN_ARGS ( ZIn_Ext_define_process_corner );

	const char *name= in_args->name();
	int model= in_args->ext_model_index();
//	bool spef= in_args->spef();

	char *cornerName= _ext->addRCCorner(name, model);
	
	if (cornerName!=NULL) {
		notice(0, "Defined Extraction corner %s\n", cornerName);
		return TCL_OK;
	}
	else {
		return TCL_ERROR;
	}
}
TCL_METHOD ( Ext::define_derived_corner )
{
    TCL_IN_ARGS ( ZIn_Ext_define_derived_corner );

	double resFactor= in_args->res_factor();
	double ccFactor= in_args->cc_factor();
	double gndcFactor= in_args->gndc_factor();
	const char *name= in_args->name();
	if (in_args->process_corner_name()==NULL) {
		warning(0, "the original process corner name is required\n");
		return TCL_ERROR;
	}

	int model= _ext->getDbCornerModel(in_args->process_corner_name());

	char *cornerName= _ext->addRCCornerScaled(name, model, resFactor, ccFactor, gndcFactor);
	
	if (cornerName!=NULL) {
		notice(0, "Defined Derived Extraction corner %s\n", cornerName);
		return TCL_OK;
	}
	else {
		return TCL_ERROR;
	}
}
TCL_METHOD ( Ext::delete_corners )
{
	_ext->deleteCorners();
	return TCL_OK;
}
TCL_METHOD ( Ext::get_corners )
{
    TCL_OUT_ARGS ( ZOut_Ext_get_corners );
	std::list<std::string> ecl;
	_ext->getCorners(ecl);
	out_args->corner_list(ecl);
    return TCL_OK;
}
TCL_METHOD ( Ext::read_qcap )
{
    TCL_IN_ARGS ( ZIn_Ext_read_qcap );

	const char *filename= in_args->file();
	const char *design= in_args->design();
	const char *capFile= in_args->cap_file();
	
	if (filename==NULL) {
		warning(0, "file option is required\n");
		return TCL_OK;
	}
        bool noqc= in_args->no_qcap();
	extMeasure m;
	if (!noqc)
		m.readQcap(_ext, filename, design, capFile, in_args->skip_bterms(), _db);
	else
		m.readAB(_ext, filename, design, capFile, in_args->skip_bterms(), _db);
	
	return TCL_OK;
}
TCL_METHOD ( Ext::get_ext_db_corner )
{
    TCL_IN_ARGS ( ZIn_Ext_get_ext_db_corner );
    TCL_OUT_ARGS ( ZOut_Ext_get_ext_db_corner );

	const char *name= in_args->name();
	
	int n= _ext->getDbCornerIndex(name);
	out_args->index(n);
	
	if (n<0)
		warning(0, "extraction corner %s not found\n", name);

	return TCL_OK;
}
TCL_METHOD ( Ext::assembly )
{
	TCL_IN_ARGS ( ZIn_Ext_assembly );
	
	dbBlock * block = (dbBlock *) in_args->block();
	dbBlock * topBlock = (dbBlock *) in_args->main_block();
	if (topBlock==NULL) {
	
		notice(0, "Add parasitics of block %s onto nets/wires ...\n", 
			block->getConstName());
	} else {
		notice(0, "Add parasitics of block %s onto block %s...\n", 
			block->getConstName(), topBlock->getConstName());
	}
	return TCL_OK;

	
	extMain::assemblyExt(topBlock, block);

	return TCL_OK;
}
TCL_METHOD ( Ext::flatten )
{
	TCL_IN_ARGS ( ZIn_Ext_flatten );
	dbBlock * block = (dbBlock *) in_args->block();
	if (block==NULL) {
		error(0, "No block for flatten command\n");
	}
	_ext->addRCtoTop(block, in_args->spef());
	return TCL_OK;
}
TCL_METHOD ( Ext::extract )
{
    TCL_IN_ARGS ( ZIn_Ext_extract );
//    fprintf(stdout, "extracting %s ...\n", _ext->getBlock()->getName().c_str());
	notice(0, "extracting %s ...\n", _ext->getBlock()->getName().c_str());

        if (in_args->lef_rc()) {
                if (!_ext->checkLayerResistance())
                        return TCL_ERROR;
                _ext->addExtModel();
                notice(0, "Using LEF RC values to extract!\n");
        }

        bool extract_power_grid_only= in_args->power_grid();
	const char *exclude_cells= in_args->exclude_cells();
        bool skip_power_stubs= in_args->skip_power_stubs();
        bool skip= in_args->skip_power_stubs();
#ifdef ZUI
	if (extract_power_grid_only)
	{
		dbBlock *block= _ext->getBlock();
    		if (block!=NULL) {
			bool skipCutVias= true;
        		block->initSearchBlock(_db->getTech(), true, true, _context, skipCutVias);
		}
		else {
                	error(0, "There is no block to extract!\n");
                        return TCL_ERROR;
		}
		_ext->setPowerExtOptions(skip_power_stubs, exclude_cells, in_args->skip_m1_caps(), in_args->power_source_coords());
	}
#endif

	const char *extRules= in_args->ext_model_file();
	const char *cmpFile= in_args->cmp_file();
	bool density_model= in_args->wire_density();
	bool litho= in_args->litho();

	uint preserve_geom= in_args->preserve_geom();
	uint ccUp= in_args->cc_up();
	uint ccFlag= in_args->cc_model();
	//uint ccFlag= 25;
	int ccBandTracks = in_args->cc_band_tracks();
	uint use_signal_table = in_args->signal_table();
	double max_res= in_args->max_res();
	bool merge_via_res= in_args->no_merge_via_res() ? false : true;
	uint extdbg= in_args->test();
	const char *bbox= in_args->bbox();
	const char *ibox= in_args->ibox();
	const char *nets= in_args->net();
	bool eco = in_args->eco();
	bool gs = in_args->no_gs() ? false : true;
	bool re_run = in_args->re_run();
	bool rlog = in_args->rlog();
	double ccThres= in_args->coupling_threshold();
	int ccContextDepth = in_args->context_depth();
	bool overCell = in_args->over_cell();
	bool btermThresholdFlag= in_args->tile();

	uint tilingDegree= in_args->tiling();

	ZPtr<ISdb> dbNetSdb= NULL;
	bool extSdb = false;

	// ccBandTracks = 0;  // tttt no band CC for now
	if (extdbg == 100 || extdbg == 102)  // 101: activate reuse metal fill
                                             // 102: no bandTrack
		ccBandTracks = 0;
	if (ccBandTracks )
		eco = false;  // tbd
	else {
#ifdef ZUI
		dbNetSdb=_ext->getBlock()->getNetSdb();

		if (dbNetSdb == NULL) {
			extSdb = true;
	    	if (rlog)
				AthResourceLog ("before get sdb", 0);

			dbNetSdb= _ext->getBlock()->getNetSdb(_context,  _ext->getTech());
			
			if (rlog)
				AthResourceLog ("after get sdb", 0);
		}
#endif
	}
	if (!_ext->checkLayerResistance())
		return TCL_ERROR;
	

	if (tilingDegree==1)
		extdbg= 501;
	else if (tilingDegree==10)
		extdbg= 603;
	else if (tilingDegree==7)
		extdbg= 703;
	else if (tilingDegree==77) {
		extdbg= 773;
		//_ext->rcGen(NULL, max_res, merge_via_res, 77, true, this);
	}
	/*
	else if (tilingDegree==77) {
		extdbg= 703;
		_ext->rcGen(NULL, max_res, merge_via_res, 77, true, this);
	}
	*/
	else if (tilingDegree==8)
		extdbg= 803;
	else if (tilingDegree==9)
		extdbg= 603;
	else if (tilingDegree==777) {
		extdbg= 777;
		
		uint cnt= 0;
		dbSet<dbNet> bnets = _ext->getBlock()->getNets();   
		dbSet<dbNet>::iterator net_itr;
		for( net_itr = bnets.begin(); net_itr != bnets.end(); ++net_itr ) {
			dbNet *net = *net_itr;
			
			cnt += _ext->rcNetGen(net);
		}
		notice(0, "777: Final rc segments = %d\n", cnt);
		dbRSeg *rc= dbRSeg::getRSeg(_ext->getBlock(), 113);

	}
		
	if (_ext->makeBlockRCsegs(btermThresholdFlag, cmpFile, density_model, litho, 
		nets, bbox, ibox, ccUp, ccFlag, ccBandTracks, use_signal_table, max_res, merge_via_res, 
		extdbg, preserve_geom, re_run, eco, gs, rlog, dbNetSdb, ccThres, ccContextDepth, overCell, extRules, this)
		==0)
		return TCL_ERROR;
	
	dbBlock* topBlock= _ext->getBlock();
	if (tilingDegree==1) {
		dbSet<dbBlock> children = topBlock->getChildren();
        dbSet<dbBlock>::iterator itr;
		
		//Extraction
			
		notice(0, "List of extraction tile blocks: \n");
        for( itr = children.begin(); itr != children.end(); ++itr )
        {
            dbBlock *blk = *itr;
			notice(0, "%s ", blk->getConstName());
		}
		notice(0, "\n");
	}
	else if (extdbg==501){
		dbSet<dbBlock> children = topBlock->getChildren();
        dbSet<dbBlock>::iterator itr;
		
		//Extraction
        for( itr = children.begin(); itr != children.end(); ++itr )
        {
            dbBlock *blk = *itr;
			notice(0, "Extacting block %s...\n", blk->getConstName());
			extMain *ext= new extMain(5);
			ext->setDB(_db);
			
			ext->setBlock(blk);
			
			if (ext->makeBlockRCsegs(btermThresholdFlag, cmpFile, density_model, litho, 
				nets, bbox, ibox, ccUp, ccFlag, ccBandTracks, use_signal_table, max_res, merge_via_res, 
				0, preserve_geom, re_run, eco, gs, rlog, dbNetSdb, ccThres, ccContextDepth, overCell, extRules, this)
				==0) {
				warning(0, "Failed to Extact block %s...\n", blk->getConstName());
				return TCL_ERROR;
			}
		}
		
        for( itr = children.begin(); itr != children.end(); ++itr )
        {
            dbBlock *blk = *itr;
			notice(0, "Assembly of block %s...\n", blk->getConstName());
			
			extMain::assemblyExt(topBlock, blk);
		}
		//_ext= new extMain(5);
		//_ext->setDB(_db);
		//_ext->setBlock(topBlock);
	}
	
	 // report total net cap

	if (! extract_power_grid_only)
	{
	char netcapfile[500];
	sprintf(netcapfile, "%s.totCap", _ext->getBlock()->getConstName());
    	_ext->reportTotalCap(netcapfile,true,false,1.0,NULL,NULL);
	}
	
	
	if (! ccBandTracks ) {
		if (extSdb && extdbg!=99) {
			if (rlog)
				AthResourceLog ("before remove sdb", 0);
			dbNetSdb->cleanSdb();
#ifdef ZUI
			_ext->getBlock()->resetNetSdb();
			if (rlog)
				AthResourceLog ("after remove sdb", 0);
#endif
		}
	}

//    fprintf(stdout, "Finished extracting %s.\n", _ext->getBlock()->getName().c_str());
    notice(0, "Finished extracting %s.\n\n", _ext->getBlock()->getName().c_str());
    return 0;
}
TCL_METHOD ( Ext::adjust_rc )
{
    TCL_IN_ARGS ( ZIn_Ext_adjust_rc );
	double resFactor= in_args->res_factor();
	double ccFactor= in_args->cc_factor();
	double gndcFactor= in_args->gndc_factor();
	_ext->adjustRC(resFactor, ccFactor, gndcFactor);
	return 0;
}
TCL_METHOD ( Ext::init_incremental_spef )
{
    TCL_IN_ARGS ( ZIn_Ext_init_incremental_spef );
	const char *origp = in_args->origp();
	const char *newp = in_args->newp();
	const char *reader = in_args->reader();
	const char *excludeCell = in_args->exclude_cells();
        bool noBackSlash = in_args->no_backslash();
	_ext->initIncrementalSpef(origp, newp, reader, excludeCell, noBackSlash);
	return 0;
}
TCL_METHOD ( Ext::export_sdb )
{
//    TCL_IN_ARGS ( ZIn_Ext_export_sdb );
    TCL_OUT_ARGS ( ZOut_Ext_export_sdb );

	out_args->cc_sdb(_ext->getCcSdb());
	out_args->net_sdb(_ext->getNetSdb());

 return 0;
}
TCL_METHOD ( Ext::attach_gui )
{
	TCL_IN_ARGS(ZIn_Ext_attach_gui);

	ZPtr<IZdcr> dcr= in_args->dcr();

	// have to add it as a member of Tmg object

	ZPtr<IZgui> igui;
	if (adsNewComponent( _context, ZCID(ZguiExt), igui )!= Z_OK) {
		assert(0);
	}
	ZALLOCATED(igui);

	igui->setDcr(dcr);

	dbBlock *block = NULL;
    dbChip *chip = _db->getChip();
    if (chip) block = chip->getBlock();
    if (!block) {
		notice(0,"need db before attach_gui onto Tmg\n");	
    } 
	else {
		//_ext->setIgui(igui);
		igui->setGuiContext(_ext, block);
    }
	dcr->addGui(igui);

	notice(0, "Attached Crawler to Ext Technology Module ...\n");

    return TCL_OK;
}
TCL_METHOD ( Ext::attach )
{
//    TCL_IN_ARGS ( ZIn_Ext_attach);
    TCL_OUT_ARGS ( ZOut_Ext_attach);

	ZPtr<IZgui> igui;
	if (adsNewComponent( _context, ZCID(ZguiExt), igui )!= Z_OK)
	{
		assert(0);
	}
	ZALLOCATED(igui);

	_ext->setIgui(igui);

	out_args->gui(igui); // in case that is needed

//	fprintf(stdout, "%s \n", _module);
	notice(0, "%s \n", _module);

//    fprintf(stdout, "Attached Crawler to Ext Technology Module ...\n");
	notice(0, "Attached Crawler to Ext Technology Module ...\n");

    return 0;
}
TCL_METHOD ( Ext::write_spef_nets )
{
    	TCL_IN_ARGS ( ZIn_Ext_write_spef_nets );

	int corner= in_args->corner();
	//const char* name= in_args->ext_corner_name();

	bool parallel= in_args->parallel();
	bool flatten= in_args->flatten();
	
	_ext->write_spef_nets(flatten, parallel);

	return 0;
}
TCL_METHOD ( Ext::write_spef )
{
    TCL_IN_ARGS ( ZIn_Ext_write_spef );

	if (in_args->end()) {
		_ext->writeSPEF(true);
		return 0;
	}
	int corner= in_args->corner();
	const char* name= in_args->ext_corner_name();

	uint netId= in_args->net_id();
	if (netId>0) {
		_ext->writeSPEF(netId, in_args->single_pi(), in_args->debug(), corner, name);
		return 0;
	}
	bool parallel= in_args->parallel();
	bool flatten= in_args->flatten();
	bool useIds= in_args->use_ids();
	bool stop= in_args->stop_after_map();
	bool initOnly= in_args->init();
	if (!initOnly)
		initOnly= parallel && flatten;
	_ext->writeSPEF((char *) in_args->file(), (char *)in_args->nets(), useIds, in_args->no_name_map(),(char *)in_args->N(), in_args->term_junction_xy(),  in_args->exclude_cells(), in_args->cap_units(), in_args->res_units(), in_args->gz(), stop, in_args->w_clock(), in_args->w_conn(), in_args->w_cap(), in_args->w_cc_cap(), in_args->w_res(), in_args->no_c_num(), initOnly, in_args->single_pi(), in_args->no_backslash(), in_args->prime_time(), in_args->psta(),
		corner, name, flatten, parallel);

    // fprintf(stdout, "Hello Extraction %s\n", "Ext::write_spef");
    return 0;
}

TCL_METHOD ( Ext::independent_spef_corner )
{
    _ext->setUniqueExttreeCorner();
    return 0;
}

TCL_METHOD ( Ext::read_spef )
{
    TCL_IN_ARGS ( ZIn_Ext_read_spef );
    // fprintf(stdout, "Hello Extraction %s\n", "Ext::read_spef");
//    fprintf(stdout, "reading %s\n",in_args->file());
	notice(0, "reading %s\n",in_args->file());

	ZPtr<ISdb> netSdb = NULL;
	bool stampWire = in_args->stamp_wire();
#ifdef ZUI
	if (stampWire)
		netSdb=_ext->getBlock()->getSignalNetSdb(_context, _db->getTech());
#endif
	bool force= in_args->force();
	bool useIds= in_args->use_ids();
	uint testParsing= in_args->test_parsing();

	Ath__parser parser;
	char *filename = (char *) in_args->file();
	if (!filename || !filename[0])
	{
		notice(0, "Please input file name.\n");
		return 0;
	}
	parser.mkWords(filename);
	
	_ext->readSPEF(parser.get(0), (char *)in_args->net(), force, useIds, in_args->r_conn(), 
	(char *)in_args->N(), in_args->r_cap(), in_args->r_cc_cap(), in_args->r_res(), in_args->cc_threshold(), in_args->cc_ground_factor(), in_args->length_unit(), in_args->m_map(),
	in_args->no_cap_num_collapse(), (char *)in_args->cap_node_map_file(), in_args->log(), in_args->corner(), 0.0, 0.0, NULL, NULL, NULL, in_args->db_corner_name(), in_args->calibrate_base_corner(), in_args->spef_corner(), in_args->fix_loop(),
		in_args->keep_loaded_corner(), stampWire, netSdb, testParsing, in_args->more_to_read(), false/*diff*/, false/*calibrate*/, in_args->app_print_limit());

	for (int ii=1; ii<parser.getWordCnt(); ii++)
		_ext->readSPEFincr(parser.get(ii));
    
    return 0;
}
TCL_METHOD ( Ext::diff_spef )
{
    TCL_IN_ARGS ( ZIn_Ext_diff_spef );
    // fprintf(stdout, "Hello Extraction %s\n", "Ext::read_spef");
//    fprintf(stdout, "diffing spef %s\n",in_args->file());
	if (in_args->file() == NULL)
	{
		notice(0, "\nplease type in name of the spef file to diff, using -file\n");
		return 0;
	}
	notice(0, "diffing spef %s\n",in_args->file());

	bool useIds= in_args->use_ids();
	bool testParsing= in_args->test_parsing();

	Ath__parser parser;
	parser.mkWords((char *) in_args->file());

	char *excludeSubWord= (char *) in_args->exclude_net_subword();
	char *subWord= (char *) in_args->net_subword();
	char *statsFile= (char *) in_args->rc_stats_file();

	_ext->readSPEF(parser.get(0), (char *)in_args->net(), false/*force*/, useIds, in_args->r_conn(), 
	NULL /*N*/, in_args->r_cap(), in_args->r_cc_cap(), in_args->r_res(), -1.0, 0.0/*cc_ground_factor*/, 1.0/*length_unit*/, in_args->m_map(),  false/*noCapNumCollapse*/, NULL/*capNodeMapFile*/, in_args->log(), 
		in_args->ext_corner(), in_args->low_guard(), in_args->upper_guard(), 
		excludeSubWord, subWord, statsFile, in_args->db_corner_name(), NULL, in_args->spef_corner(), 0/*fix_loop*/,
		false/*keepLoadedCorner*/, false/*stampWire*/, NULL/*netSdb*/, testParsing, false/*moreToRead*/, true/*diff*/, false/*calibrate*/, 0);

	//for (uint ii=1; ii<parser.getWordCnt(); ii++)
	//	_ext->readSPEFincr(parser.get(ii));
    
    return 0;
}
TCL_METHOD ( Ext::calibrate )
{
    TCL_IN_ARGS ( ZIn_Ext_calibrate );
	if (in_args->spef_file() == NULL)
	{
		notice(0, "\nplease type in name of the spef file to calibrate, using -spef_file\n");
		return 0;
	}
	notice(0, "calibrate on spef file  %s\n",in_args->spef_file());
	Ath__parser parser;
	parser.mkWords((char *) in_args->spef_file());
	_ext->calibrate(parser.get(0), in_args->m_map(), in_args->upper_limit(), in_args->lower_limit(),
		in_args->db_corner_name(), in_args->corner(), in_args->spef_corner());
    return 0;
}
TCL_METHOD ( Ext::match )
{
    TCL_IN_ARGS ( ZIn_Ext_match );
	if (in_args->spef_file() == NULL)
	{
		notice(0, "\nplease type in name of the spef file to match, using -spef_file\n");
		return 0;
	}
	notice(0, "match on spef file  %s\n",in_args->spef_file());
	Ath__parser parser;
	parser.mkWords((char *) in_args->spef_file());
	_ext->match(parser.get(0), in_args->m_map(),
		in_args->db_corner_name(), in_args->corner(), in_args->spef_corner());
    return 0;
}
#if 0
TCL_METHOD ( Ext::setRules )
{
    ZIn_Ext_setRules * in_args = (ZIn_Ext_setRules *) in;
	_extModel= (ExtModel *) in_args->model();
	xm->getFringeCap(....)
}
#endif
TCL_METHOD ( Ext::set_block )
{
    TCL_IN_ARGS( ZIn_Ext_set_block );
    dbBlock *block = (dbBlock *) in_args->block();
    dbInst *inst = (dbInst *) in_args->inst();
    const char *inst_name= in_args->inst_name();
    if (inst_name!=NULL)
    {
        dbChip *chip= _db->getChip();
	dbInst *ii = chip->getBlock()->findInst(inst_name);
	dbMaster *m= ii->getMaster();
	notice(0, "Inst=%s ==> %d %s of Master %d %s", inst_name, ii->getId(), ii->getConstName(), m->getId(), m->getConstName());
    }
        if (block==NULL) {
                const char *block_name= in_args->block_name();
		
                if (block_name==NULL) {
                        warning(0, "commnad requires either dbblock or block name\n");
                        return TCL_ERROR;
                }
                dbChip *chip= _db->getChip();
                dbBlock * child = chip->getBlock()->findChild(block_name);
                if (child==NULL) {
                        warning(0, "Cannot find block with master name %s\n",
                                block_name);
                        return TCL_ERROR;
                }
                block= child;
        }

	delete _ext;
	_ext= new extMain(5);
	_ext->setDB(_db);

    _ext->setBlock(block);
    return 0;
}
TCL_METHOD ( Ext::report_total_cap )
{
    TCL_IN_ARGS( ZIn_Ext_report_total_cap );
    bool cap = in_args->cap_only();
    bool res = in_args->res_only();
    _ext->reportTotalCap(in_args->file(),cap,res,in_args->ccmult(),in_args->ref(),in_args->read());
/*
	dbChip *chip= _db->getChip();
	dbBlock * child = chip->getBlock()->findChild(block_name);
if (child==NULL) {
warning(0, "Cannot find block with master name %s\n",
block_name);
return TCL_ERROR;
}
block= child;
*/
    return 0;
}
TCL_METHOD ( Ext::report_total_cc )
{
    TCL_IN_ARGS( ZIn_Ext_report_total_cc );
    _ext->reportTotalCc(in_args->file(),in_args->ref(),in_args->read());
    return 0;
}
TCL_METHOD ( Ext::dump)
{
    TCL_IN_ARGS ( ZIn_Ext_dump );
    // fprintf(stdout, "Hello Extraction %s\n", "Ext::read_spef");
//    fprintf(stdout, "dumping %s\n",in_args->file());
    notice(0, "dumping %s\n",in_args->file());

    bool openTreeFile = in_args->open_tree_file();
    bool closeTreeFile = in_args->close_tree_file();
    bool ccCapGeom = in_args->cc_cap_geom();
    bool ccNetGeom = in_args->cc_net_geom();
    bool trackcnt = in_args->track_cnt();
    bool signal = in_args->signal();
    bool power = in_args->power();
    uint layer= in_args->layer();
    _ext->extDump((char *) in_args->file(), openTreeFile, closeTreeFile, ccCapGeom, ccNetGeom, trackcnt, signal, power, layer);
    
    return 0;
}
TCL_METHOD ( Ext::count)
{
    TCL_IN_ARGS ( ZIn_Ext_count );

    bool signalWireSeg = in_args->signal_wire_seg();
    bool powerWireSeg = in_args->power_wire_seg();
    _ext->extCount(signalWireSeg, powerWireSeg);
    
    return 0;
}
TCL_METHOD ( Ext::rc_tree )
{
    TCL_IN_ARGS ( ZIn_Ext_rc_tree );
	int netId= in_args->net();
	uint test= (uint) in_args->test();
	float max_cap= in_args->max_cap();
	char *printTag= (char *)in_args->print_tag();

	dbBlock* block= _ext->getBlock();

	if (_tree==NULL)
		_tree= new extRcTree(block);

	uint cnt;
	if (netId>0)
		_tree->makeTree((uint) netId, max_cap, test, true, true, cnt, 1.0/*mcf*/, printTag, false/*for_buffering*/);
	else
		_tree->makeTree(max_cap, test);

	return TCL_OK;

}
TCL_METHOD ( Ext::net_stats )
{
    TCL_IN_ARGS ( ZIn_Ext_net_stats );
	TCL_OUT_ARGS ( ZOut_Ext_net_stats );
	
	Ath__parser parser;
	extNetStats limits;
	limits.reset();
	
	limits.update_double(&parser, in_args->tcap(), limits._tcap);
	limits.update_double(&parser, in_args->ccap(), limits._ccap);
	limits.update_double(&parser, in_args->ratio_cap(), limits._cc2tcap);
	limits.update_double(&parser, in_args->res(), limits._res);
	
	limits.update_int(&parser, in_args->len(), limits._len, 1000);
	limits.update_int(&parser, in_args->met_cnt(), limits._layerCnt);
	limits.update_int(&parser, in_args->wire_cnt(), limits._wCnt);
	limits.update_int(&parser, in_args->via_cnt(), limits._vCnt);
	limits.update_int(&parser, in_args->term_cnt(), limits._termCnt);
	limits.update_int(&parser, in_args->bterm_cnt(), limits._btermCnt);
	limits.update_bbox(&parser, in_args->bbox());
	
	FILE *fp= stdout;
	const char* filename= in_args->file();
	if (filename!=NULL) {
		fp= fopen(filename, "w");
		if (fp==NULL) {
			warning(0, "Cannot open file %s\n", filename);
			return TCL_OK;	
		}
	}
	bool skipDb= false;
	bool skipRC= false;
	bool skipPower= true;

	dbBlock* block= _ext->getBlock();
	if (block==NULL) {
		warning(0, "There is no extracted block\n");
		skipRC= true;
		return TCL_OK;	
	}
	std::list<int> list_of_nets;
	int n= _ext->printNetStats(fp, block, &limits, skipRC, skipDb, skipPower, &list_of_nets);
	notice(0, "%d nets found\n", n);

	out_args->net_ids(list_of_nets);

	return TCL_OK;	
}
END_NAMESPACE_ADS
