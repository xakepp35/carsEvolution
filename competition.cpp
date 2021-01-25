#include "competition.h"

#include <numeric>
#include <algorithm>


competition::competition(size_t participantCount) {
	rankingChart.resize(participantCount);
	std::iota(rankingChart.begin(), rankingChart.end(), 0);

	accumulatedScore.resize(participantCount);
	std::fill(accumulatedScore.begin(), accumulatedScore.end(), 0);

	accumulatedCost.resize(participantCount);
	std::fill(accumulatedCost.begin(), accumulatedCost.end(), 1);
}


const competition::ranking_chart& competition::competition_step(i_solver& iSolver, i_problem& iProblem) {
	// replace worst efforts (due to ineficiency or misbehaving) / initialize data (if this is first call, due to zero cost efficiency)
	perform_evolution(iSolver, iProblem);

	// basing on current situation, predict control decision
	iProblem.describe_situation(participant_count());
	translate_situation(iSolver, iProblem);
	iSolver.predict_decision(accumulatedCost, participant_count());

	// basing on predicted decision, perform some actual step 
	translate_decision(iProblem, iSolver);
	iProblem.update_situation(accumulatedScore, participant_count());

	// sort by cost efficiency, after actions were done and scores/costs were updated
	return sort_ranking_chart();
}


size_t competition::participant_count() const {
	return rankingChart.size();
}


bool competition::compare_cost_efficiency(score_t scoreI, score_t costI, score_t scoreJ, score_t costJ) {
	return (scoreI*costJ) > (scoreJ*costI);
}


const competition::ranking_chart& competition::sort_ranking_chart() {
	std::sort(rankingChart.begin(), rankingChart.end(), [this](size_t i, size_t j) {
		return compare_cost_efficiency(accumulatedScore[i], accumulatedCost[i], accumulatedScore[j], accumulatedCost[j]);
	});
	return rankingChart;
}

int ddd = 0;

bool competition::is_inefficient(size_t i, i_solver& iSolver, i_problem& iProblem) const {
	return (accumulatedScore[i] < 0) || (accumulatedCost[i] > accumulatedScore[i]);
}

void competition::perform_evolution(i_solver& iSolver, i_problem& iProblem) {
	auto bestExists = !is_inefficient(rankingChart[0], iSolver, iProblem);
	for (auto& i : rankingChart) {
		if (is_inefficient(i, iSolver, iProblem)) { // situation is deadly or decision was too expensive
			iSolver.new_predictor(i, rankingChart, bestExists);
			accumulatedCost[i] = 1;
			iProblem.new_performer(i, rankingChart, bestExists);
			accumulatedScore[i] = 0; // even first step have to be efficient, or attempt fail on next check;
		}
	}
}
