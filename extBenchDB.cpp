/* Copyright Athena Design Systems 2005 */

#include "extRCap.h"
#include "extprocess.h"
#include "extSpef.h"

#ifdef _WIN32
#include "direct.h"
#endif

#include <vector>
#include <map>
//#include "logger.h"
#include <logger.h>

BEGIN_NAMESPACE_ADS

uint extRCModel::benchDB_WS(extMainOptions* opt, extMeasure* measure)
{
	Ath__array1D<double>* wTable = &opt->_widthTable;
	Ath__array1D<double>* sTable = &opt->_spaceTable;
	Ath__array1D<double>* gTable = &opt->_gridTable;

	uint cnt = 0;
	int met = measure->_met;
	dbTechLayer* layer = opt->_tech->findRoutingLayer(met);

	double minWidth = 0.001 * layer->getWidth();;
	double pitch = 0.001 * layer->getPitch();
	// double spacing = 0.001 * layer->getSpacing(); TODO
	double spacing = pitch - minWidth;

	if (opt->_default_lef_rules) { // minWidth, minSpacing, minThickness, pitch multiplied by grid_list
		wTable->resetCnt();
		sTable->resetCnt();
		for (uint ii = 0; ii < gTable->getCnt(); ii++) {
			// double s = minWidth + pitch * (gTable->get(ii) - 1);
			double s = spacing * gTable->get(ii);
			sTable->add(s);
			double w = minWidth * gTable->get(ii);
			wTable->add(w);
		}
	}
	else if (opt->_nondefault_lef_rules) {
		return 0;
		wTable->resetCnt();
		sTable->resetCnt();
		dbSet<dbTechNonDefaultRule> nd_rules = opt->_tech->getNonDefaultRules();
		dbSet<dbTechNonDefaultRule>::iterator nditr;
		dbTechLayerRule* tst_rule;
		//		dbTechNonDefaultRule  *wdth_rule = NULL;

		for (nditr = nd_rules.begin(); nditr != nd_rules.end(); ++nditr) {
			tst_rule = (*nditr)->getLayerRule(layer);
			if (tst_rule == NULL)
				continue;

			double w = tst_rule->getWidth();
			double s = tst_rule->getSpacing();
			wTable->add(w);
			sTable->add(s);
		}
	}
	bool use_symmetric_widths_spacings = false;
	if (!use_symmetric_widths_spacings) {
		for (uint ii = 0; ii < wTable->getCnt(); ii++) {
			double w = wTable->get(ii); // layout
			double w2 = w;
			for (uint jj = 0; jj < sTable->getCnt(); jj++) {
				double s = sTable->get(jj); // layout
				double s2 = s;

				measure->setTargetParams(w, s, 0.0, 0, 0, w2, s2);
				// measureResistance(measure, ro, top_widthR, bot_widthR, thicknessR);
				// measurePatternVar(measure, top_width, bot_width, thickness, measure->_wireCnt, NULL);
				writeBenchWires_DB(measure);

				cnt++;
			}
		}
	}
	else {
	/* REQUIRED Testing
		for (uint ii = 0; ii < wTable->getCnt(); ii++) {
			double w = wTable->get(ii); // layout
			for (uint iii = 0; iii < wTable->getCnt(); iii++) {
				double w2 = wTable->get(iii);
				for (uint jj = 0; jj < sTable->getCnt(); jj++) {
					double s = sTable->get(jj); // layout
					for (uint jjj = 0; jjj < sTable->getCnt(); jjj++) {
						double s2 = sTable->get(jjj);

						for (uint kk = 0; kk < thTable->getCnt(); kk++) {
							double tt = thTable->get(kk); // layout
							if (!opt->_thListFlag) // multiplier
								tt *= t;

							double top_width = w;
							double top_widthR = w;

							double bot_width = w;
							double bot_widthR = w;

							double thickness = tt;
							double thicknessR = tt;


							measure->setTargetParams(w, s, 0.0, t, h, w2, s2);
							measureResistance(measure, ro, top_widthR, bot_widthR, thicknessR);
							measurePatternVar(measure, top_width, bot_width, thickness, measure->_wireCnt, NULL);

							cnt++;
						}
					}
				}
			}
		}
		*/
	}
	return cnt;
}
int extRCModel::writeBenchWires_DB(extMeasure* measure)
{
	mkFileNames(measure, "");
	uint grid_gap_cnt = 20;

	int bboxLL[2];
	bboxLL[measure->_dir] = measure->_ur[measure->_dir];
	bboxLL[!measure->_dir] = measure->_ll[!measure->_dir];

	int n = measure->_wireCnt / 2; // ASSUME odd number of wires, 2 will also work

	double pitchUp_print= measure->_topWidth;
	double pitch_print = 0.001 * (measure->_minWidth + measure->_minSpace);

	uint w_layout = measure->_minWidth;
	uint s_layout = measure->_minSpace;

	double x = -(measure->_topWidth * 0.5 + pitchUp_print + pitch_print);

	measure->clean2dBoxTable(measure->_met, false);

	double x_tmp[50];
	uint netIdTable[50];
	uint idCnt = 1;
	int ii;
	for (ii = 0; ii < n - 1; ii++) {
		netIdTable[idCnt] = measure->createNetSingleWire(_wireDirName, idCnt, w_layout, s_layout);
		idCnt++;
		x_tmp[ii] = x;
		x -= pitch_print;
	}

	double X[50];
	ii--;
	int cnt = 0;
	for (; ii >= 0; ii--)
		X[cnt++] = x_tmp[ii];

	uint WW = measure->_w_nm;
	uint SS1;
	//	if (measure->_diag)
	//		SS1= 2*measure->_minSpace;
	//	else
	SS1 = measure->_s_nm;
	uint WW2 = measure->_w2_nm;
	uint SS2 = measure->_s2_nm;

	X[cnt++] = -pitchUp_print; int mid = cnt;
	netIdTable[idCnt] = measure->createNetSingleWire(_wireDirName, idCnt, WW, s_layout);
	idCnt++;

	X[cnt++] = 0.0;
	netIdTable[idCnt] = measure->createNetSingleWire(_wireDirName, idCnt, WW, SS1);
	idCnt++;
	uint base = measure->_ll[measure->_dir] + WW / 2;

	X[cnt++] = (SS2 + WW * 0.5) * 0.001;
	netIdTable[idCnt] = measure->createNetSingleWire(_wireDirName, idCnt, WW2, SS2);
	idCnt++;

	//	x= measure->_topWidth*0.5+pitchUp_print+0.001*measure->_minSpace; 
	x = measure->_topWidth * 0.5 + 0.001 * (WW2 + SS2 + measure->_minSpace);
	for (int jj = 0; jj < n - 1; jj++) {
		X[cnt++] = x;
		x += pitch_print;
		netIdTable[idCnt] = measure->createNetSingleWire(_wireDirName, idCnt, w_layout, s_layout);
		idCnt++;
	}

	if (measure->_diag) {
		int met;
		if (measure->_overMet > 0)
			met = measure->_overMet;
		else if (measure->_underMet > 0)
			met = measure->_underMet;

		double minWidth = measure->_minWidth;
		double minSpace = measure->_minSpace;
		double min_pitch = minWidth + minSpace;
		measure->clean2dBoxTable(met, false);
		int i;
		uint begin = base - Ath__double2int(measure->_seff * 1000) + Ath__double2int(minWidth * 1000) / 2;
		for (i = 0; i < n + 1; i++) {
			netIdTable[idCnt] = measure->createDiagNetSingleWire(_wireDirName, idCnt, begin, Ath__double2int(1000 * minWidth), Ath__double2int(1000 * minSpace), measure->_dir);
			begin -= Ath__double2int(min_pitch * 1000);
			idCnt++;
		}
		measure->_ur[measure->_dir] += grid_gap_cnt * (w_layout + s_layout);
		return cnt;
	}

	int bboxUR[2] = { measure->_ur[0], measure->_ur[1] };

	double pitchMult = 1.0;

	measure->clean2dBoxTable(measure->_underMet, true);
	measure->createContextNets(_wireDirName, bboxLL, bboxUR, measure->_underMet, pitchMult);

	measure->clean2dBoxTable(measure->_overMet, true);
	measure->createContextNets(_wireDirName, bboxLL, bboxUR, measure->_overMet, pitchMult);

	//	double mainNetStart= X[0];
	int main_xlo, main_ylo, main_xhi, main_yhi, low;
	measure->getBox(measure->_met, false, main_xlo, main_ylo, main_xhi, main_yhi);
	if (!measure->_dir)
		low = main_ylo - measure->_minWidth / 2;
	else
		low = main_xlo - measure->_minWidth / 2;
	uint contextRaphaelCnt = 0;
	if (measure->_underMet > 0) {
		// double h = _process->getConductor(measure->_underMet)->_height;
		// double t = _process->getConductor(measure->_underMet)->_thickness;
		/* KEEP -- REMINDER
				dbTechLayer *layer=measure->_tech->findRoutingLayer(_underMet);
				uint minWidth= layer->getWidth();
				uint minSpace= layer->getSpacing();
				uint pitch= 1000*((minWidth+minSpace)*pitchMult)/1000;
				uint offset= 2*(minWidth+minSpace);
				int start= mainNetStart+offset;
				contextRaphaelCnt= measure->writeRaphael3D(fp, measure->_underMet, true, mainNetStart, h, t);
		*/
		// contextRaphaelCnt = measure->writeRaphael3D(fp, measure->_underMet, true, low, h, t);
	}

	if (measure->_overMet > 0) {
		// double h = _process->getConductor(measure->_overMet)->_height;
		// double t = _process->getConductor(measure->_overMet)->_thickness;
		/* KEEP -- REMINDER
				dbTechLayer *layer=measure->_tech->findRoutingLayer(_overMet);
						uint minWidth= layer->getWidth();
						uint minSpace= layer->getSpacing();
						uint pitch= 1000*((minWidth+minSpace)*pitchMult)/1000;
						uint offset= 2*(minWidth+minSpace);
						int start= mainNetStart+offset;
				contextRaphaelCnt += measure->writeRaphael3D(fp, measure->_overMet, true, mainNetStart, h, t);
		*/
		// contextRaphaelCnt += measure->writeRaphael3D(fp, measure->_overMet, true, low, h, t);
	}

	measure->_ur[measure->_dir] += grid_gap_cnt * (w_layout + s_layout);
	return cnt;
}

END_NAMESPACE_ADS
