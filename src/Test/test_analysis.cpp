#include <boost/test/unit_test.hpp>

#include "../analysis.h"
#include "../log_file.h"

//#define SLOW_TEST
#ifdef SLOW_TEST

static Setting setting1;

void InitializeSetting1()
{
	setting1.lz = ToString(R"(C:\bin\leela-zero-0.16-cpuonly-win64\leelaz.exe)");
	setting1.networkWeights = ToString(R"(C:\bin\leela-zero-0.16-cpuonly-win64\elf1.gz)");
	setting1.numberOfThreads = 2;
	setting1.numberOfVisits = 30;
	setting1.numberOfProblems = 12;
	setting1.start = 5;
	setting1.last = 244;
	setting1.maximumNumberOfMoves = 20;
	setting1.player = Player::BLACK;
	setting1.playerNames.clear();
	setting1.searchSpeed = 0.25;
	setting1.boardSize = 19;
}

static const char* const sgfA =
"(;GM[1]FF[4]SZ[19]CA[UTF-8]KM[0]RE[W+R]DT[1901-01-20]\n"
";B[qd];W[dd];B[nc];W[qp];B[co];W[dq];B[op];W[oq];B[nq];W[pq]\n"
";B[np];W[po];B[gq];W[ep];B[jp];W[cl];B[qj];W[gc];B[qn];W[pn]\n"
";B[qm];W[dn];B[bq];W[cn];B[be];W[cd];B[ch];W[cf];B[bf];W[cg]\n"
";B[bg];W[bi];B[bh];W[eg];B[eh];W[fh];B[bc];W[bd];B[ad];W[cb]\n"
";B[bj];W[ci];B[dh];W[cj];B[fi];W[fg];B[ej];W[er];B[ne];W[rd]\n"
";B[rc];W[qe];B[pd];W[qh];B[nr];W[qo];B[rf];W[oh];B[ol];W[nn]\n"
";B[oj];W[mh];B[jc];W[iq];B[jq];W[ip];B[io];W[jr];B[hp];W[ir]\n"
";B[gr];W[kq];B[kp];W[lq];B[lp];W[ho];B[lr];W[kr];B[mq];W[ls]\n"
";B[mr];W[hn];B[hq];W[gi];B[gj];W[fk];B[hi];W[gh];B[fj];W[hj]\n"
";B[cc];W[dc];B[bb];W[db];B[gk];W[ij];B[ba];W[pm];B[ql];W[jd]\n"
";B[kd];W[ke];B[kc];W[lf];B[id];W[je];B[hc];W[gd];B[ie];W[if]\n"
";B[lj];W[ah];B[fl];W[hl];B[in];W[im];B[dk];W[ck];B[dm];W[cm]\n"
";B[ni];W[nh];B[ki];W[ld];B[me];W[le];B[lc];W[md];B[nd];W[nk]\n"
";B[mi];W[re];B[qf];W[pf];B[pe];W[qg];B[of];W[pg];B[sd];W[rg]\n"
";B[se];W[rk];B[pl];W[rj];B[kn];W[el];B[dl];W[em];B[en];W[fm]\n"
";B[gl];W[gm];B[ek];W[fs];B[hs];W[gp];B[fp];W[go];B[fq];W[fo]\n"
";B[is];W[eq];B[ks];W[ll];B[ml];W[nl];B[om];W[nm];B[on];W[lk]\n"
";B[jl];W[jo];B[jn];W[jm];B[qr];W[ps];B[pr];W[or];B[qq];W[pp]\n"
";B[rp];W[ro];B[sq];W[so];B[rr];W[oo];B[no];W[sl];B[lh];W[jh]\n"
";B[sm];W[rm];B[rl];W[sn];B[sk];W[sj];B[mg];W[mf];B[ng];W[lg]\n"
";B[og];W[ri];B[oi];W[ph];B[sh];W[sp];B[qs];W[os];B[ss];W[rq]\n"
";B[mk];W[lm];B[rp];W[nf];B[oe];W[rq];B[km];W[kl];B[rp];W[kh]\n"
";B[rq];W[li];B[kj];W[ln];B[ko];W[kk];B[lh];W[pj])\n";

BOOST_AUTO_TEST_CASE(Analysis_Test)
{
	InitializeSetting1();

	JobList jobList;
	jobList.jobs.push_back(Job());
	auto& job = jobList.jobs.front();
	job.clear();
	job.game.readSgf(sgfA, "UTF-8");
	job.setPlayer(setting1);
	job.setNumberOfProblems(setting1);

	SearchSpeed searchSpeed;
	searchSpeed.clear(setting1.searchSpeed);

	std::atomic<bool> ss(false);
	const std::string id = "test_id";
	int number = 1;
	ExitStatus exitStatus = ExitStatus::FAILED;

	Log.open("analysis1_log.txt");
	Analysis analysis1;
	auto html = analysis1.generateHtml(setting1, jobList,
		searchSpeed, ss, "analysis", id, exitStatus);
	Log.close();

	Log.open("analysis1_log.html");
	Log(html);
	Log.close();

	BOOST_CHECK(true);
}

#endif
