#define PLAYER_SETUP_POINTS 1200

#define VALID_STARTUP 4
#define CUSTOM_STARTUP 0

#define TEMPLATES 8
#define MAX_TEMPLATE_HEX 22
#define DEFAULT_STARTUP 3 /* which one the players get stuck with if they
			     fail to allocate valid home den */

#define TEMPLATE1_HEX 7
#define TEMPLATE2_HEX 9
#define TEMPLATE3_HEX 11
#define TEMPLATE4_HEX 14
#define TEMPLATE5_HEX 16
#define TEMPLATE6_HEX 17
#define TEMPLATE7_HEX 19
#define TEMPLATE8_HEX 22

#define TEMPLATE1_COST 100
#define TEMPLATE2_COST 110
#define TEMPLATE3_COST 120
#define TEMPLATE4_COST 140
#define TEMPLATE5_COST 150
#define TEMPLATE6_COST 160
#define TEMPLATE7_COST 180
#define TEMPLATE8_COST 200

#define RUIN_COST 40
#define PLAIN_COST 15
#define SWAMP_COST 10
#define SCRUB_COST 10
#define FERTILE_COST 25
#define PEAK_COST 10
#define VOLCANO_COST 10
#define TEMPLE_COST 40
#define CURSED_COST 15
#define WHIRLPOOL_COST 5
#define WATER_COST 0

#define GROUP_SIZE 10		/* lizards are dealt with in 10 groups */

#define RED_WARRIOR_COST 10	/* all per 10 pop lizards */
#define YELLOW_WARRIOR_COST 15
#define GREEN_WARRIOR_COST 20
#define GREY_WARRIOR_COST 20
#define BLACK_WARRIOR_COST 30

#define RED_POP_COST 15 	/* all per 10 pop lizards */
#define YELLOW_POP_COST 25
#define GREEN_POP_COST 25
#define GREY_POP_COST 30
#define BLACK_POP_COST 50

#define MINIMUM_POP_PER_DEN 3	/* 3 groups of 10 pop lizards! */

#define RED_DEN_COST 65 + (MINIMUM_POP_PER_DEN * RED_POP_COST)
#define YELLOW_DEN_COST 100 + (MINIMUM_POP_PER_DEN * YELLOW_POP_COST)
#define GREEN_DEN_COST 110 + (MINIMUM_POP_PER_DEN * GREEN_POP_COST)
#define GREY_DEN_COST 95 + (MINIMUM_POP_PER_DEN * GREY_POP_COST)
#define BLACK_DEN_COST 150 + (MINIMUM_POP_PER_DEN * BLACK_POP_COST)
