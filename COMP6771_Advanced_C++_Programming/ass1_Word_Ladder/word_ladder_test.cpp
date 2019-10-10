/*

  == Explanation and rational of testing ==

  Explain and justify how you approached testing, the degree
   to which you're certain you have covered all possibilities,
   and why you think your tests are that thorough.

*/
#include <string>

#include "assignments/wl/word_ladder.h"
#include "assignments/wl/lexicon.h"
#include "catch.h"

// TODO(students): Fill this in.

TEST_CASE("word ladder testing", "[word-ladder]") {
  auto lexicon = GetLexicon("assignments/wl/words.txt");
  std::string start = "work";
  std::string goal = "play";
  REQUIRE(Bfs(lexicon, start, goal) == "Found ladder: work fork form foam flam flay play \n"
                                       "work pork perk peak pean plan play \n"
                                       "work pork perk peak peat plat play \n"
                                       "work pork perk pert peat plat play \n"
                                       "work pork porn pirn pian plan play \n"
                                       "work pork port pert peat plat play \n"
                                       "work word wood pood plod ploy play \n"
                                       "work worm form foam flam flay play \n"
                                       "work worn porn pirn pian plan play \n"
                                       "work wort bort boat blat plat play \n"
                                       "work wort port pert peat plat play \n"
                                       "work wort wert pert peat plat play \n");
  start = "awake";
  goal = "sleep";
  REQUIRE(Bfs(lexicon, start, goal) ==
          "Found ladder: awake aware sware share sharn shawn shewn sheen sheep sleep \n"
          "awake aware sware share shire shirr shier sheer sheep sleep \n");

  start = "code";
  goal = "gate";
  REQUIRE(Bfs(lexicon, start, goal) == "Found ladder: code cade cate gate \n"
                                       "code cote cate gate \n");

  start = "letters";
  goal = "teacher";
  REQUIRE(Bfs(lexicon, start, goal) == "Found ladder: letters petters putters puttees putties "
                                       "puttier pettier peatier peakier beakier brakier brasier "
                                       "brasher brashes braches beaches teaches teacher \n");
}
