/* 96K sampling, 3K transition, 80 stopband */
enum { kaiser_96K_taps = 163 };


/* 46K cutoff + 1.5K window */
static buf_t const kaiser_96K_48K[kaiser_96K_taps] = {
LPF_TAPS(-0.000009705697569455),
LPF_TAPS(0.000013044726805146),
LPF_TAPS(-0.000016294401403681),
LPF_TAPS(0.000019122350598294),
LPF_TAPS(-0.000021120288327786),
LPF_TAPS(0.000021808357498738),
LPF_TAPS(-0.000020644056638012),
LPF_TAPS(0.000017036124255331),
LPF_TAPS(-0.000010363589553793),
LPF_TAPS(0.000000000000000000),
LPF_TAPS(0.000014657389365078),
LPF_TAPS(-0.000034153934746875),
LPF_TAPS(0.000058940151279375),
LPF_TAPS(-0.000089331475414456),
LPF_TAPS(0.000125466492228020),
LPF_TAPS(-0.000167265093538493),
LPF_TAPS(0.000214388254680309),
LPF_TAPS(-0.000266201277687037),
LPF_TAPS(0.000321742459629520),
LPF_TAPS(-0.000379699194440990),
LPF_TAPS(0.000438393495886875),
LPF_TAPS(-0.000495778831987043),
LPF_TAPS(0.000549449983221321),
LPF_TAPS(-0.000596667377005808),
LPF_TAPS(0.000634397010929198),
LPF_TAPS(-0.000659366661909873),
LPF_TAPS(0.000668138595770006),
LPF_TAPS(-0.000657198452827751),
LPF_TAPS(0.000623059404020050),
LPF_TAPS(-0.000562380065451674),
LPF_TAPS(0.000472094045996779),
LPF_TAPS(-0.000349548403214566),
LPF_TAPS(0.000192647718995589),
LPF_TAPS(-0.000000000000000021),
LPF_TAPS(-0.000228939819338234),
LPF_TAPS(0.000493733321305249),
LPF_TAPS(-0.000792833756934858),
LPF_TAPS(0.001123477742042483),
LPF_TAPS(-0.001481598264422932),
LPF_TAPS(0.001861763546157840),
LPF_TAPS(-0.002257145924330587),
LPF_TAPS(0.002659524376797930),
LPF_TAPS(-0.003059323633665363),
LPF_TAPS(0.003445691991674653),
LPF_TAPS(-0.003806619004911404),
LPF_TAPS(0.004129093182997299),
LPF_TAPS(-0.004399298713536580),
LPF_TAPS(0.004602849069050333),
LPF_TAPS(-0.004725054192865575),
LPF_TAPS(0.004751216818313310),
LPF_TAPS(-0.004666952396924675),
LPF_TAPS(0.004458526129691198),
LPF_TAPS(-0.004113199745097672),
LPF_TAPS(0.003619579980244203),
LPF_TAPS(-0.002967960224998689),
LPF_TAPS(0.002150646507126041),
LPF_TAPS(-0.001162258946482250),
LPF_TAPS(0.000000000000000061),
LPF_TAPS(0.001336118739249708),
LPF_TAPS(-0.002843098739125900),
LPF_TAPS(0.004514832350784100),
LPF_TAPS(-0.006342041747808949),
LPF_TAPS(0.008312283144126405),
LPF_TAPS(-0.010410018962976157),
LPF_TAPS(0.012616759016324575),
LPF_TAPS(-0.014911270059945742),
LPF_TAPS(0.017269851359396383),
LPF_TAPS(-0.019666672183921433),
LPF_TAPS(0.022074165487436865),
LPF_TAPS(-0.024463470485978332),
LPF_TAPS(0.026804915445090276),
LPF_TAPS(-0.029068530790722628),
LPF_TAPS(0.031224581690543835),
LPF_TAPS(-0.033244108550304914),
LPF_TAPS(0.035099463455959573),
LPF_TAPS(-0.036764830482636568),
LPF_TAPS(0.038216717993662677),
LPF_TAPS(-0.039434411565134900),
LPF_TAPS(0.040400376983550638),
LPF_TAPS(-0.041100603856539054),
LPF_TAPS(0.041524881722335025),
LPF_TAPS(0.958341048470601642),
LPF_TAPS(0.041524881722335025),
LPF_TAPS(-0.041100603856539054),
LPF_TAPS(0.040400376983550638),
LPF_TAPS(-0.039434411565134900),
LPF_TAPS(0.038216717993662677),
LPF_TAPS(-0.036764830482636568),
LPF_TAPS(0.035099463455959573),
LPF_TAPS(-0.033244108550304914),
LPF_TAPS(0.031224581690543835),
LPF_TAPS(-0.029068530790722628),
LPF_TAPS(0.026804915445090276),
LPF_TAPS(-0.024463470485978332),
LPF_TAPS(0.022074165487436865),
LPF_TAPS(-0.019666672183921416),
LPF_TAPS(0.017269851359396383),
LPF_TAPS(-0.014911270059945742),
LPF_TAPS(0.012616759016324575),
LPF_TAPS(-0.010410018962976157),
LPF_TAPS(0.008312283144126405),
LPF_TAPS(-0.006342041747808949),
LPF_TAPS(0.004514832350784100),
LPF_TAPS(-0.002843098739125900),
LPF_TAPS(0.001336118739249709),
LPF_TAPS(0.000000000000000061),
LPF_TAPS(-0.001162258946482250),
LPF_TAPS(0.002150646507126041),
LPF_TAPS(-0.002967960224998689),
LPF_TAPS(0.003619579980244203),
LPF_TAPS(-0.004113199745097672),
LPF_TAPS(0.004458526129691194),
LPF_TAPS(-0.004666952396924675),
LPF_TAPS(0.004751216818313310),
LPF_TAPS(-0.004725054192865575),
LPF_TAPS(0.004602849069050333),
LPF_TAPS(-0.004399298713536580),
LPF_TAPS(0.004129093182997299),
LPF_TAPS(-0.003806619004911404),
LPF_TAPS(0.003445691991674651),
LPF_TAPS(-0.003059323633665363),
LPF_TAPS(0.002659524376797930),
LPF_TAPS(-0.002257145924330587),
LPF_TAPS(0.001861763546157840),
LPF_TAPS(-0.001481598264422932),
LPF_TAPS(0.001123477742042483),
LPF_TAPS(-0.000792833756934858),
LPF_TAPS(0.000493733321305249),
LPF_TAPS(-0.000228939819338235),
LPF_TAPS(-0.000000000000000021),
LPF_TAPS(0.000192647718995589),
LPF_TAPS(-0.000349548403214566),
LPF_TAPS(0.000472094045996779),
LPF_TAPS(-0.000562380065451674),
LPF_TAPS(0.000623059404020050),
LPF_TAPS(-0.000657198452827751),
LPF_TAPS(0.000668138595770006),
LPF_TAPS(-0.000659366661909873),
LPF_TAPS(0.000634397010929198),
LPF_TAPS(-0.000596667377005808),
LPF_TAPS(0.000549449983221322),
LPF_TAPS(-0.000495778831987043),
LPF_TAPS(0.000438393495886875),
LPF_TAPS(-0.000379699194440990),
LPF_TAPS(0.000321742459629520),
LPF_TAPS(-0.000266201277687037),
LPF_TAPS(0.000214388254680309),
LPF_TAPS(-0.000167265093538493),
LPF_TAPS(0.000125466492228020),
LPF_TAPS(-0.000089331475414456),
LPF_TAPS(0.000058940151279375),
LPF_TAPS(-0.000034153934746875),
LPF_TAPS(0.000014657389365078),
LPF_TAPS(0.000000000000000000),
LPF_TAPS(-0.000010363589553793),
LPF_TAPS(0.000017036124255331),
LPF_TAPS(-0.000020644056638012),
LPF_TAPS(0.000021808357498738),
LPF_TAPS(-0.000021120288327786),
LPF_TAPS(0.000019122350598294),
LPF_TAPS(-0.000016294401403681),
LPF_TAPS(0.000013044726805146),
LPF_TAPS(-0.000009705697569455),
};


/* 22K cutoff + 1.5K window */
static buf_t const kaiser_96K_24K[kaiser_96K_taps] = {
LPF_TAPS(-0.000004020165606453),
LPF_TAPS(0.000013044512783016),
LPF_TAPS(0.000012502928813678),
LPF_TAPS(-0.000019122036861849),
LPF_TAPS(-0.000027524044041795),
LPF_TAPS(0.000021807999693537),
LPF_TAPS(0.000049838343817088),
LPF_TAPS(-0.000017035844747170),
LPF_TAPS(-0.000078717986444941),
LPF_TAPS(0.000000000000000000),
LPF_TAPS(0.000111332098918946),
LPF_TAPS(0.000034153374390338),
LPF_TAPS(-0.000142291777996970),
LPF_TAPS(-0.000089330009771446),
LPF_TAPS(0.000163508433419932),
LPF_TAPS(0.000167262349254758),
LPF_TAPS(-0.000164503194707794),
LPF_TAPS(-0.000266196910177824),
LPF_TAPS(0.000133267903835064),
LPF_TAPS(0.000379692964794971),
LPF_TAPS(-0.000057714651731711),
LPF_TAPS(-0.000495770697846463),
LPF_TAPS(-0.000072335275781076),
LPF_TAPS(0.000596657587607774),
LPF_TAPS(0.000262771534546967),
LPF_TAPS(-0.000659355843817669),
LPF_TAPS(-0.000512672364797407),
LPF_TAPS(0.000657187670308880),
LPF_TAPS(0.000811973502006641),
LPF_TAPS(-0.000562370838598473),
LPF_TAPS(-0.001139717149151074),
LPF_TAPS(0.000349542668246372),
LPF_TAPS(0.001463280695730761),
LPF_TAPS(-0.000000000000000016),
LPF_TAPS(-0.001738942043374988),
LPF_TAPS(-0.000493725220724951),
LPF_TAPS(0.001914038604949264),
LPF_TAPS(0.001123459309375883),
LPF_TAPS(-0.001930824771391703),
LPF_TAPS(-0.001861733000589071),
LPF_TAPS(0.001731940567489314),
LPF_TAPS(0.002659480742532465),
LPF_TAPS(-0.001267192549845997),
LPF_TAPS(-0.003445635458920007),
LPF_TAPS(0.000501142677081268),
LPF_TAPS(0.004129025437820916),
LPF_TAPS(0.000579169160805724),
LPF_TAPS(-0.004602773551059283),
LPF_TAPS(-0.001957149418560915),
LPF_TAPS(0.004751138866083349),
LPF_TAPS(0.003581019771759925),
LPF_TAPS(-0.004458452979576717),
LPF_TAPS(-0.005360338323972023),
LPF_TAPS(0.003619520594545454),
LPF_TAPS(0.007165172268349895),
LPF_TAPS(-0.002150611221914444),
LPF_TAPS(-0.008828088329801986),
LPF_TAPS(0.000000000000000047),
LPF_TAPS(0.010148662899004075),
LPF_TAPS(0.002843052092993787),
LPF_TAPS(-0.010899590662844032),
LPF_TAPS(-0.006341937695243880),
LPF_TAPS(0.010832600568516570),
LPF_TAPS(0.010409848167951803),
LPF_TAPS(-0.009681020856943260),
LPF_TAPS(-0.014911025413827180),
LPF_TAPS(0.007153289288768493),
LPF_TAPS(0.019666349516905442),
LPF_TAPS(-0.002906071338485945),
LPF_TAPS(-0.024463069118886436),
LPF_TAPS(-0.003528876167474964),
LPF_TAPS(0.029068053869360417),
LPF_TAPS(0.012933433016012430),
LPF_TAPS(-0.033243563121103499),
LPF_TAPS(-0.026932323693034201),
LPF_TAPS(0.036764227289679798),
LPF_TAPS(0.049804179415798483),
LPF_TAPS(-0.039433764572914462),
LPF_TAPS(-0.097533537799871201),
LPF_TAPS(0.041099929527446899),
LPF_TAPS(0.315407616210535369),
LPF_TAPS(0.458329503341011468),
LPF_TAPS(0.315407616210535369),
LPF_TAPS(0.041099929527446899),
LPF_TAPS(-0.097533537799871201),
LPF_TAPS(-0.039433764572914462),
LPF_TAPS(0.049804179415798483),
LPF_TAPS(0.036764227289679798),
LPF_TAPS(-0.026932323693034201),
LPF_TAPS(-0.033243563121103499),
LPF_TAPS(0.012933433016012430),
LPF_TAPS(0.029068053869360417),
LPF_TAPS(-0.003528876167474964),
LPF_TAPS(-0.024463069118886436),
LPF_TAPS(-0.002906071338485945),
LPF_TAPS(0.019666349516905428),
LPF_TAPS(0.007153289288768493),
LPF_TAPS(-0.014911025413827180),
LPF_TAPS(-0.009681020856943260),
LPF_TAPS(0.010409848167951803),
LPF_TAPS(0.010832600568516570),
LPF_TAPS(-0.006341937695243880),
LPF_TAPS(-0.010899590662844032),
LPF_TAPS(0.002843052092993787),
LPF_TAPS(0.010148662899004084),
LPF_TAPS(0.000000000000000047),
LPF_TAPS(-0.008828088329801986),
LPF_TAPS(-0.002150611221914444),
LPF_TAPS(0.007165172268349895),
LPF_TAPS(0.003619520594545454),
LPF_TAPS(-0.005360338323972023),
LPF_TAPS(-0.004458452979576714),
LPF_TAPS(0.003581019771759925),
LPF_TAPS(0.004751138866083349),
LPF_TAPS(-0.001957149418560915),
LPF_TAPS(-0.004602773551059283),
LPF_TAPS(0.000579169160805724),
LPF_TAPS(0.004129025437820916),
LPF_TAPS(0.000501142677081268),
LPF_TAPS(-0.003445635458920005),
LPF_TAPS(-0.001267192549845997),
LPF_TAPS(0.002659480742532465),
LPF_TAPS(0.001731940567489314),
LPF_TAPS(-0.001861733000589071),
LPF_TAPS(-0.001930824771391703),
LPF_TAPS(0.001123459309375883),
LPF_TAPS(0.001914038604949264),
LPF_TAPS(-0.000493725220724951),
LPF_TAPS(-0.001738942043374990),
LPF_TAPS(-0.000000000000000016),
LPF_TAPS(0.001463280695730761),
LPF_TAPS(0.000349542668246372),
LPF_TAPS(-0.001139717149151074),
LPF_TAPS(-0.000562370838598473),
LPF_TAPS(0.000811973502006641),
LPF_TAPS(0.000657187670308880),
LPF_TAPS(-0.000512672364797407),
LPF_TAPS(-0.000659355843817669),
LPF_TAPS(0.000262771534546967),
LPF_TAPS(0.000596657587607774),
LPF_TAPS(-0.000072335275781076),
LPF_TAPS(-0.000495770697846463),
LPF_TAPS(-0.000057714651731711),
LPF_TAPS(0.000379692964794971),
LPF_TAPS(0.000133267903835064),
LPF_TAPS(-0.000266196910177824),
LPF_TAPS(-0.000164503194707794),
LPF_TAPS(0.000167262349254758),
LPF_TAPS(0.000163508433419932),
LPF_TAPS(-0.000089330009771446),
LPF_TAPS(-0.000142291777996970),
LPF_TAPS(0.000034153374390338),
LPF_TAPS(0.000111332098918946),
LPF_TAPS(0.000000000000000000),
LPF_TAPS(-0.000078717986444941),
LPF_TAPS(-0.000017035844747170),
LPF_TAPS(0.000049838343817088),
LPF_TAPS(0.000021807999693537),
LPF_TAPS(-0.000027524044041795),
LPF_TAPS(-0.000019122036861849),
LPF_TAPS(0.000012502928813678),
LPF_TAPS(0.000013044512783016),
LPF_TAPS(-0.000004020165606453),
};
