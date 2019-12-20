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

#ifndef ADS_EXTSPEF_H
#define ADS_EXTSPEF_H

#ifndef ADS_H
//#include "ads.h"
#include <ads.h>
#endif

#include "db.h"
//#include "dbShape.h"
#include <dbShape.h>
//#include "array1.h"
//#include "parse.h"
//#include "name.h"
#include <array1.h>
#include <parse.h>
#include <name.h>

//#include "ISdb.h"
#include <ISdb.h>

//#define AFILE FILE


#include <map>

BEGIN_NAMESPACE_ADS

class extRcTripplet
{
private:
	uint _srcId;
	uint _dstId;
	uint _dstNetId;
	char _dstWord[32];
	char _valueWord[64];
	bool _coupleFlag;
	
	friend class extSpef;
};


class extSpef
{
private:
	enum COORD_TYPE
	{
		C_NONE,
		C_STARRC,
		C_MAGMA
	} ;

	char _inFile[1024];
	FILE *_inFP;

	char _outFile[1024];
//	AFILE *_outFP;
	FILE *_outFP;

	Ath__parser *_parser;

	Ath__parser *_nodeParser;
	Ath__parser *_nodeCoordParser;
	uint _tmpNetSpefId;

	dbTech *_tech;
	dbBlock *_block;
	dbBlock *_cornerBlock;
	bool _useBaseCornerRc;
	uint _blockId;

	char _design[1024];
	char _bus_delimiter[5];
	char _delimiter[5];
	char _divider[5];

	char _res_unit_word[5];
	char _cap_unit_word[5];
	char _time_unit_word[5];
	char _ind_unit_word[7];

	double _res_unit;
	double _cap_unit;
	int _time_unit;
	int _ind_unit;

	uint _cornerCnt;
	uint _cornersPerBlock;

	bool _extracted;

	Ath__array1D<uint> *_nodeTable;
	Ath__array1D<uint> *_btermTable;
	Ath__array1D<uint> *_itermTable;
	Ath__array1D<double*> *_nodeCapTable;
	Ath__array1D<double*> *_btermCapTable;
	Ath__array1D<double*> *_itermCapTable;

	char *_spefName;
	uint _maxMapId;
	Ath__array1D<uint> *_idMapTable;
	Ath__array1D<char*> *_nameMapTable;
	uint _lastNameMapIndex;

	uint _cCnt;
	uint _rCnt;

	bool _partial;
	bool _btermFound;

	bool _noBackSlash;
	bool _primeTime;
	bool _psta;

	uint _baseNameMap;
	uint _firstCapNode;
	bool _useIds; // Net/inst/bterm names expected to be look like : N1, I1, B1
		// name is same as in save_def option

	bool _preserveCapValues;
	bool _symmetricCCcaps;

	bool _testParsing;

	uint _tnetCnt;

	uint _wRun;
	bool _wOnlyClock;
	bool _wConn;
	bool _wCap;
	bool _wOnlyCCcap;
	bool _wRes;
	bool _noCnum;
	bool _foreign;

	uint _rRun;
	ZPtr<ISdb> _netSdb;
	bool _stampWire;
	bool _rConn;
	bool _rCap;
	bool _rOnlyCCcap;
	bool _rRes;
	bool _mMap;
	bool _noNameMap;
	bool _noPorts;
	bool _keep_loaded_corner;
	char _mMapName[2000];
	char _nDvdName[2000];
	bool _inputNet;
	
	Ath__nameTable *_notFoundInst;
	Ath__nameTable *_nodeHashTable;
	//AthHash<uint> *_nodeHashTable;
	//HashN<uint> _nodeHashTable;
	uint _tmpCapId;
	Ath__nameTable *_node2nodeHashTable;
	char _tmpBuff1[1024];
	char _tmpBuff2[1024];

	uint _gndCapCnt;
	uint _ccCapCnt;
	uint _resCnt;
	bool _statsOnly;


	AthPool<extRcTripplet> *_rcPool;
	Ath__array1D<extRcTripplet*> *_rcTrippletTable;

	uint _maxNetNode;
	uint _minNetNode;

	bool _gzipFlag;
	bool _stopAfterNameMap;
	float _upperCalibLimit;
	float _lowerCalibLimit;
	bool _calib;
	bool _diff;
	bool _match;
	dbNet *_d_corner_net;
	dbNet *_d_net;
	uint _unmatchedSpefNet;
	uint _unmatchedSpefInst;
	FILE *_diffLogFP;
	FILE *_diffOutFP;
	uint _diffLogCnt;
	double _upperThres;
	double _lowerThres;

	double _netCCapTable[10];
	double _netGndCapTable[10];
	double _netResTable[10];

	bool _stopBeforeDnets;
	double _cc_thres;
	bool _cc_thres_flag;
	uint _cc_break_cnt;
	uint _cc_merge_cnt;
	uint _cc_app_print_limit;
	int _appcnt[16];
	Ath__array1D<int> *_ccidmap;

	uint _dbCorner;
	char *_tmpNetName;
	char *_netSubWord;
	char *_netExcludeSubWord;
	FILE *_capStatsFP;

	bool _singleP;
	bool _NsLayer;

	std::vector<dbNet *> _netV1;

	Ath__array1D<uint>*	_capNodeTable;
	Ath__array1D<double>* _xCoordTable;
	Ath__array1D<double>* _yCoordTable;
	Ath__array1D<int>* _x1CoordTable;
	Ath__array1D<int>* _y1CoordTable;
	Ath__array1D<int>* _x2CoordTable;
	Ath__array1D<int>* _y2CoordTable;
	Ath__array1D<uint>* _levelTable;
	Ath__gridTable *_search;
	Ath__array1D<uint>*	_idTable;
	double _lengthUnit;
	double _nodeCoordFactor;
	bool _doSortRSeg;
	COORD_TYPE _readingNodeCoordsInput;
	COORD_TYPE _readingNodeCoords;
	COORD_TYPE _writingNodeCoords;

	int _fixloop;
	uint _breakLoopNet;
	uint _loopNet;
	uint _bigLoop;
	uint _multipleLoop;
	uint _srii;
	Ath__array1D<uint> *_srsegi;
	Ath__array1D<dbRSeg *> *_nrseg;
	Ath__array1D< Ath__array1D<int> *> *_hcnrc;
	uint _rsegCnt;

	bool _readAllCorners;
	int _in_spef_corner;

	//021810D BEGIN
	uint _childBlockInstBaseMap;
	uint _childBlockNetBaseMap;
	//021810D END
public:
	bool _addRepeatedCapValue;
	bool _noCapNumCollapse;
	FILE *_capNodeFile;
	int _db_calibbase_corner;
	int _db_ext_corner;
	int _active_corner_cnt;
	int _active_corner_number[32];
	bool _writeNameMap;
	bool _moreToRead;
	bool _termJxy;
	bool _independentExtCorners;
	bool _incrPlusCcNets;
	dbBTerm *_ccbterm1;
	dbBTerm *_ccbterm2;
	dbITerm *_cciterm1;
	dbITerm *_cciterm2;
	char *_bufString;
	char *_msgBuf1;
	char *_msgBuf2;


public:
	extSpef(dbTech *tech, dbBlock *blk, uint btermCnt=0, uint itermCnt=0);
	~extSpef();
	
	bool matchNetGndCap(dbNet *net, uint dbCorner, double dbCap, double refCap);
	bool calibrateNetGndCap(dbNet *net, uint dbCorner, double dbCap, double refCap);
	bool computeFactor(double db, double ref, float &factor);

	void initNodeCoordTables(uint memChunk);
	void resetNodeCoordTables();
	void deleteNodeCoordTables();
	bool readNodeCoords(uint cpos);
	void adjustNodeCoords();
	void checkCCterm();
	int findNodeIndexFromNodeCoords(uint targetCapNodeId);
	uint getShapeIdFromNodeCoords(uint targetCapNodeId);
	uint getITermShapeId(dbITerm *iterm);
	uint getBTermShapeId(dbBTerm *bterm);
	void initSearchForNets();
	uint addNetShapesOnSearch(uint netId);
	uint findShapeId(uint netId, int x, int y);
	uint parseAndFindShapeId();
	void readNmCoords();
	uint findShapeId(uint netId, int x1, int y1, int x2, int y2, char *layer, bool matchLayer=false);
	uint findShapeId(uint netId, int x1, int y1, int x2, int y2, uint level);
	void searchDealloc();
	void getAnchorCoords(dbNet *net, uint shapeId, int *x1, int *y1, int *x2, int *y2, dbTechLayer **layer);
	uint writeNodeCoords(uint netId, dbSet<dbRSeg> & rSet);

	void setupMappingForWrite(uint btermCnt=0, uint itermCnt=0);
	void setupMapping(uint itermCnt=0);
	void preserveFlag(bool v);
	void setCornerCnt(uint n);

	void incr_wRun(){_wRun++;};
	void incr_rRun(){_rRun++;};
	void writeCNodeNumber();
	dbBlock *getBlock() { return _block; }
	uint writeNet(dbNet *net, double resBound, uint debug);
	uint stopWrite();
	uint readBlock(uint debug, std::vector<dbNet *> tnets, bool force, bool rConn, char *nodeCoord, bool rCap, bool rOnlyCCcap, bool rRes, float cc_thres, float length_unit, bool extracted, bool keepLoadedCorner, bool stampWire, ZPtr<ISdb> netSdb, uint testParsing, int app_print_limit, bool m_map, int corner, double low, double up, char *excludeNetSubWord, char *netSubWord, char *capStatsFile,
 const char *dbCornerName, const char *calibrateBaseCorner, int spefCorner, int fixLoop, bool & resegCoord);
	uint readBlockIncr(uint debug);

	bool setOutSpef(char *filename);
	bool closeOutFile();
	void setGzipFlag(bool gzFlag);
	bool setInSpef(char *filename, bool onlyOpen=false);
	bool isCapNodeExcluded(dbCapNode *node);
	uint writeBlock(char *nodeCoord, const char* excludeCell, const char* capUnit, const char* resUnit, bool stopAfterNameMap, std::vector<dbNet *> * tnets, bool  wClock, bool wConn, bool wCap, bool wOnlyCCcap, bool wRes,  bool noCnum, bool stopBeforeDnets, bool noBackSlash, bool primeTime, bool psta, bool flatten, bool parallel);
	uint writeBlock(char *nodeCoord, const char* excludeCell, const char* capUnit, const char* resUnit, bool stopAfterNameMap, std::vector<dbNet *> tnets, bool  wClock, bool wConn, bool wCap, bool wOnlyCCcap, bool wRes,  bool noCnum, bool stopBeforeDnets, bool noBackSlash, bool primeTime, bool psta, bool flatten, bool parallel);

	int getWriteCorner(int corner, const char *name);
	bool writeITerm(uint node);
	bool writeBTerm(uint node);
	bool writeNode(uint netId, uint node);
	uint writePort(uint node);
	void writeDnet(uint netId, double *totCap);
	void writeKeyword(char *keyword);
	uint writePorts();
	uint writeITerms();
	uint getInstMapId(uint id);
	void writeITermNode(uint node);

	bool isBTermMarked(uint node);
	void markBTerm(uint node);
	void unMarkBTerm(uint node);
	uint getBTermMapping(uint b);

	uint getNetMapId(uint netId);
	char *tinkerSpefName(char *iname);

	bool isITermMarked(uint node);
	void markITerm(uint node);
	void unMarkITerm(uint node);
	uint getITermMapping(uint b);
	void resetTermTables();
	void resetCapTables(uint maxNode);
	void initCapTable(Ath__array1D<double*> *table);
	void deleteTableCap(Ath__array1D<double*> *table);
	void reinitCapTable(Ath__array1D<double*> *table, uint n);
	void addCap(double *cap, double *totCap, uint n);
	void addHalfCap(double *totCap, double *cap, uint n=0);
	void getCaps(dbNet *net, double *totCap);
	void resetCap(double *cap);
	void resetCap(double *cap, uint cnt);
	void writeRCvalue(double *totCap, double units);
	uint writeCapPort(uint index);
	uint writeCapITerm(uint index);
	uint writeCapITerms();
	uint writeCapPorts();
	void writeNodeCaps(uint netId, uint minNode, uint maxNode);
	uint writeBlockPorts();
	uint writeNetMap( dbSet<dbNet> & nets);
	uint writeInstMap();
	
	uint readDNet(uint debug);
	uint getSpefNode(char *nodeWord, uint *instNetId, int *nodeType);
	uint getITermId(uint instId, char *name);
	uint getBTermId(char *name);
	uint getBTermId(uint id);

	bool readHeaderInfo(uint debug, bool skip=false);
	bool writeHeaderInfo(uint debug);
	bool readPorts(uint debug);
	bool readNameMap(uint debug, bool skip=false);

	void setDesign(char *name);
	uint getCapNode(char *nodeWord, char *capWord);
	uint getMappedBTermId(uint id);
	void setUseIdsFlag(bool useIds, bool diff=false, bool calib=false);
	void setCalibLimit(float upperLimit, float lowerLimit);
	uint diffGndCap(dbNet *net, uint capCnt, uint capId);
	uint diffNetCcap(dbNet *net);
	double printDiff(dbNet *net, double dbCap, double refCap, const char *ctype, int ii, int id=-1);
	uint diffCCap(dbNet *srcNet, uint srcId, dbNet *tgtNet, uint dstId, uint capCnt);
	double printDiffCC(dbNet *net1, dbNet *net2, uint node1, uint node2, double dbCap, double refCap, const char *ctype, int ii);
	uint diffNetCap(dbNet *net);
	uint diffNetGndCap(dbNet *net);
	uint diffNetRes(dbNet *net);
	uint matchNetRes(dbNet *net);
	void resetExtIds(uint rit);
	uint endNet(dbNet *net, uint resCnt);
	void setJunctionId(dbCapNode *capnode, dbRSeg *rseg);
	uint sortRSegs();
	bool getFirstShape(dbNet *net, dbShape & s);
	uint getNetLW(dbNet *net, uint & w);
	bool mkCapStats(dbNet *net);
	void collectDbCCap(dbNet *net);
	void matchCcValue(dbNet *net);
	uint matchNetCcap(dbNet *net);
	uint collectRefCCap(dbNet *srcNet, dbNet *tgtNet, uint capCnt);

	uint getNodeCap(dbSet<dbRSeg> & rcSet, uint capNodeId, double *totCap);
	uint getMultiples(uint cnt, uint base);
	uint readMaxMapId(int *cornerCnt=NULL);
	void addNameMapId(uint ii, uint id);
	uint getNameMapId(uint ii);
	void setCap(double *cap, uint n, double *totCap, uint startIndex);
	void incrementCounter(double *cap, uint n);
	uint setRCCaps(dbNet *net);

	uint getMinCapNode(dbNet *net, uint *minNode);
	uint computeCaps(dbSet<dbRSeg> & rcSet, double *totCap);
	uint getMappedCapNode(uint nodeId);
	uint writePorts(dbNet *net);
	uint writeITerms(dbNet *net);
	uint writeCapPorts(dbNet *net);
	uint writeCapITerms(dbNet *net);
	uint writeNodeCaps(dbNet *net, uint netId=0);
	uint writeCapPort(uint node, uint capIndex);
	uint writeCapITerm(uint node, uint capIndex);
	void writeNodeCap(uint netId, uint capIndex, uint ii);
	uint writeRes(uint netId, dbSet<dbRSeg> & rcSet);
	bool writeCapNode(uint capNodeId, uint netId);
	bool writeCapNode(dbCapNode *capNode, uint netId);
	uint getCapNodeId(char *nodeWord, char *capWord, uint *netId);
	uint getCapNodeId(char *nodeWord);
	uint getCapIdFromCapTable(char *nodeWord);
	void addNewCapIdOnCapTable(char *nodeWord, uint capId);
	uint getItermCapNode(uint termId);
	uint getBtermCapNode(uint termId);
	uint writeSrcCouplingCapsNoSort(dbNet *net);
	uint writeTgtCouplingCapsNoSort(dbNet *net);
	uint writeSrcCouplingCaps(dbNet *net, uint netId=0);
	uint writeTgtCouplingCaps(dbNet *net, uint netId=0);
	uint writeCouplingCaps(std::vector<dbCCSeg *> & vec_cc, uint netId);
	uint writeCouplingCaps(dbSet<dbCCSeg> & capSet, uint netId);
	uint writeCouplingCapsNoSort(dbSet<dbCCSeg> & capSet, uint netId);
	bool newCouplingCap(char *nodeWord1, char *nodeword2, char *capWord);
	uint getCouplingCapId(uint ccNode1, uint ccNode2);
	void addCouplingCapId(uint ccId);
	void setSpefFlag(bool v);
	void setExtIds(dbNet *net);
	void setExtIds();
	void resetNameTable(uint n);
	void createName(uint n, char *name);
	char* makeName(char *name);
	dbNet *getDbNet(uint *id, uint spefId=0);
	dbInst *getDbInst(uint id);
	dbCapNode *createCapNode(uint nodeId, char *capWord=NULL);
	void addCouplingCaps(dbNet *net, double *totCap);
	void addCouplingCaps(dbSet<dbCCSeg> & capSet, double *totCap);
	uint writeCapPortsAndIterms(dbSet<dbCapNode> & capSet, bool bterms);
	void writeSingleRC(double val, bool delimeter);
	uint writeInternalCaps(dbNet *net, dbSet<dbCapNode> & capSet);
	void printCapNode (uint capNodeId);
	void printAppearance(int app, int appc);
	void printAppearance(int *appcnt, int tapp);

	void reinit();
	void addNetNodeHash(dbNet * net);
	void buildNodeHashTable();

	bool isNetExcluded();


	uint computeCapsAdd2Target(dbSet<dbRSeg> & rcSet, double *totCap);
	void copyCap(double *totCap, double *cap, uint n=0);
	void adjustCap(double *totCap, double *cap, uint n=0);
	void set_single_pi(bool v);
	//uint getCapNodeId(std::map<uint,uint> &node_map, char *nodeWord, char *capWord);

	uint getAppPrintLimit() { return _cc_app_print_limit; };
	int *getAppCnt() { return _appcnt;};
#if 0
	bool writeITerm(uint node);
	bool writeBTerm(uint node);
	void writeITermNode(uint node);
	uint getMappedBTermId(uint id);
	uint getITermId(uint id, char *name);
	uint getITermId(char *name);
	uint getBTermId(uint id);
	uint getBTermId(char *name);
#endif
	//021610D BEGIN
	uint writeHierInstNameMap();
	uint writeHierNetNameMap();
	static int getIntProperty(dbBlock *block, const char *name);
	uint write_spef_nets(bool flatten, bool parallel);
	char *getDelimeter();
	void writeNameNode(dbCapNode *node);
	uint writeCapName(dbCapNode *capNode, uint capIndex);
	//021610D END

	//021810D BEGIN
	void writeDnetHier(uint mapId, double *totCap);
	bool writeHierNet(dbNet *net, double resBound, uint debug);
	void setHierBaseNameMap(uint instBase, uint netBase);
	//021810D END

	//022310D BEGIN
	void setBlock(dbBlock *blk);
	//022310D END
};

END_NAMESPACE_ADS

#endif



