/*
	Cost-Efficiency-based competition/evolution abstract engine

	Date : Jun 2018
	Author : xakepp35@gmail.com
	License : FreeBSD(ISC) for non - commercial(personal, educational) use.
*/
#pragma once
#include <cstdint>
#include <vector>

// main logic class
class competition
{
public:

	typedef size_t index_t;
	typedef int64_t score_t;

	typedef std::vector< index_t > ranking_chart;
	typedef std::vector< index_t > score_chart;


	class i_solver {
	public:

		// predicts some control solution, or "decision" for forthcoming problem step, and updates spent costs
		virtual void predict_decision(score_chart& accumulatedCost, size_t participantCount) = 0;

		// replace misbehaving predictor with a new one, rankingChart[0] is most-valued predictor
		virtual void new_predictor(size_t misbehavingIndex, const ranking_chart& rankingChart) = 0;

	};

	class i_problem {
	public:

		// describes current actual situation, that data is to be fed to solver's predictor
		virtual void describe_situation(size_t participantCount) = 0;

		// performs actual step, updates "situation" and scores. to forcibly "kill" agent you may set negative score
		virtual void update_situation(score_chart& accumulatedScore, size_t participantCount) = 0;

		// replace misbehaving performer with a new one, rankingChart[0] is best-valued performer
		virtual void new_performer(size_t misbehavingIndex, const ranking_chart& rankingChart) = 0;

	};

	// because solver and problem may operate in very different terms and data formats
	// following interface serves as a glue layer/normalisator/data converter 
	// (with knowledge of particular problem and solver implementation details). 
	// all data between those moves here:

	// get decision data from solver, convert it to format, sutiable for problem control input
	virtual void translate_decision(i_problem& iProblem, const i_solver& iSolver) const = 0;

	// get situation data from problem, convert it to format, sutiable for solver control input
	virtual void translate_situation(i_solver& iSolver, const i_problem& iProblem) const = 0;


	// initializes competition engine
	competition(size_t participantCount);

	// main pipeline: its output is sorted rankingChart, which contains indices in descending order of score/cost ratio
	const ranking_chart& competition_step(i_solver& iSolver, i_problem& iProblem);

	// returns total participants count
	size_t competition::participant_count() const;


protected:

	// rankingChart , sorted after every step; i rely on std::sort method which should be fast
	const ranking_chart& sort_ranking_chart();

	// participant fails when its score/cost ratio < 1(computationally inefficient) or its score is negative(did something really bad and died)
	void perform_evolution(i_solver& iSolver, i_problem& iProblem);// , score_t costThreshold = 0);

	// (score[i]/cost[i]) > (score[j]/cost[j]), optimized for integers
	static bool compare_cost_efficiency(score_t scoreA, score_t costA, score_t scoreB, score_t costB);

	// contains indices, sorted in cost-efficiency descending order, so that for each entry pair : (score[i]/cost[i]) > (score[j]/cost[j])
	ranking_chart	rankingChart;

	// investments in competitors (computational complexity or whatever spent resources)
	score_chart		accumulatedScore;

	// competitors' score (desired effort, or valuable outcome)
	score_chart		accumulatedCost;

};