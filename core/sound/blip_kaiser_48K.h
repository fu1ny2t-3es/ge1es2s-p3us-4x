/* 48K sampling, 3K transition, 80 stopband */
enum { kaiser_48K_taps = 83 };


/* 22K cutoff + 1.5K window */
static buf_t const kaiser_48K_24K[kaiser_48K_taps] = {
LPF_TAPS(-0.000020047417211002),
LPF_TAPS(0.000034891657335677),
LPF_TAPS(-0.000047905300032708),
LPF_TAPS(0.000052491245716357),
LPF_TAPS(-0.000039880954523092),
LPF_TAPS(0.000000000000000000),
LPF_TAPS(0.000077023002723741),
LPF_TAPS(-0.000198756908097787),
LPF_TAPS(0.000367953679327243),
LPF_TAPS(-0.000579920332491189),
LPF_TAPS(0.000820186242572323),
LPF_TAPS(-0.001062943482811212),
LPF_TAPS(0.001270747425938473),
LPF_TAPS(-0.001395901508198200),
LPF_TAPS(0.001383808452892187),
LPF_TAPS(-0.001178356028593485),
LPF_TAPS(0.000729136109458152),
LPF_TAPS(-0.000000000000000033),
LPF_TAPS(-0.001021832255465703),
LPF_TAPS(0.002317124200415146),
LPF_TAPS(-0.003827593741462540),
LPF_TAPS(0.005451674914308387),
LPF_TAPS(-0.007044112660424600),
LPF_TAPS(0.008420151972923564),
LPF_TAPS(-0.009364667504005608),
LPF_TAPS(0.009646064868031013),
LPF_TAPS(-0.009034222290057398),
LPF_TAPS(0.007321194486562898),
LPF_TAPS(-0.004342937731719087),
LPF_TAPS(0.000000000000000094),
LPF_TAPS(0.005724995618412504),
LPF_TAPS(-0.012755130970461060),
LPF_TAPS(0.020913941468382127),
LPF_TAPS(-0.029928272453658911),
LPF_TAPS(0.039439651557148285),
LPF_TAPS(-0.049023761418357036),
LPF_TAPS(0.058216767167032356),
LPF_TAPS(-0.066546509086359340),
LPF_TAPS(0.073565999071179666),
LPF_TAPS(-0.078886329608300540),
LPF_TAPS(0.082206060393380215),
LPF_TAPS(0.916678436236979977),
LPF_TAPS(0.082206060393380215),
LPF_TAPS(-0.078886329608300540),
LPF_TAPS(0.073565999071179666),
LPF_TAPS(-0.066546509086359340),
LPF_TAPS(0.058216767167032356),
LPF_TAPS(-0.049023761418357036),
LPF_TAPS(0.039439651557148285),
LPF_TAPS(-0.029928272453658911),
LPF_TAPS(0.020913941468382127),
LPF_TAPS(-0.012755130970461060),
LPF_TAPS(0.005724995618412504),
LPF_TAPS(0.000000000000000094),
LPF_TAPS(-0.004342937731719087),
LPF_TAPS(0.007321194486562891),
LPF_TAPS(-0.009034222290057393),
LPF_TAPS(0.009646064868031013),
LPF_TAPS(-0.009364667504005613),
LPF_TAPS(0.008420151972923564),
LPF_TAPS(-0.007044112660424600),
LPF_TAPS(0.005451674914308387),
LPF_TAPS(-0.003827593741462540),
LPF_TAPS(0.002317124200415146),
LPF_TAPS(-0.001021832255465703),
LPF_TAPS(-0.000000000000000033),
LPF_TAPS(0.000729136109458152),
LPF_TAPS(-0.001178356028593485),
LPF_TAPS(0.001383808452892187),
LPF_TAPS(-0.001395901508198200),
LPF_TAPS(0.001270747425938473),
LPF_TAPS(-0.001062943482811212),
LPF_TAPS(0.000820186242572323),
LPF_TAPS(-0.000579920332491189),
LPF_TAPS(0.000367953679327243),
LPF_TAPS(-0.000198756908097787),
LPF_TAPS(0.000077023002723741),
LPF_TAPS(0.000000000000000000),
LPF_TAPS(-0.000039880954523092),
LPF_TAPS(0.000052491245716357),
LPF_TAPS(-0.000047905300032708),
LPF_TAPS(0.000034891657335677),
LPF_TAPS(-0.000020047417211002),
};
