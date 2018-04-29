#ifndef DLIB_DNn_SEMANTIC_SEGMENTATION_EX_H_
#define DLIB_DNn_SEMANTIC_SEGMENTATION_EX_H_

#include <dlib/dnn.h>

// ----------------------------------------------------------------------------------------

/*
inline bool operator == (const dlib::rgb_pixel& a, const dlib::rgb_pixel& b)
{
    return a.red == b.red && a.green == b.green && a.blue == b.blue;
}
*/

// ----------------------------------------------------------------------------------------

// The PASCAL VOC2012 dataset contains 20 ground-truth classes + background.  Each class
// is represented using an RGB color value.  We associate each class also to an index in the
// range [0, 20], used internally by the network.

struct SegDataset {
    SegDataset(uint16_t index, const std::string& classlabel)
        : index(index), classlabel(classlabel)
    {}

    // The index of the class. In the PASCAL VOC 2012 dataset, indexes from 0 to 20 are valid.
    const uint16_t index = 0;

    // The corresponding RGB representation of the class.
    //const dlib::rgb_pixel rgb_label;

    // The label of the class in plain text.
    const std::string classlabel;
};

int getClassFromFile(char* sfilename, std::vector<SegDataset>& classes);

namespace {
  	constexpr int class_count = 151;
	std::vector<SegDataset> classes = { 
		SegDataset(0, ""),
		SegDataset(dlib::loss_multiclass_log_per_pixel_::label_to_ignore, "border"),
		SegDataset(1,"wall"),
		SegDataset(2,"building; edifice"),
		SegDataset(3,"sky"),
		SegDataset(4,"floor; flooring"),
		SegDataset(5,"tree"),
		SegDataset(6,"ceiling"),
		SegDataset(7,"road; route"),
		SegDataset(8,"bed "),
		SegDataset(9,"windowpane; window "),
		SegDataset(10,"grass"),
		SegDataset(11,"cabinet"),
		SegDataset(12,"sidewalk; pavement"),
		SegDataset(13,"person; individual; someone; somebody; mortal; soul"),
		SegDataset(14,"earth; ground"),
		SegDataset(15,"door; double door"),
		SegDataset(16,"table"),
		SegDataset(17,"mountain; mount"),
		SegDataset(18,"plant; flora; plant life"),
		SegDataset(19,"curtain; drape; drapery; mantle; pall"),
		SegDataset(20,"chair"),
		SegDataset(21,"car; auto; automobile; machine; motorcar"),
		SegDataset(22 ,"water"),
		SegDataset(23,"painting; picture"),
		SegDataset(24 ,"sofa; couch; lounge"),
		SegDataset(25 ,"shelf"),
		SegDataset(26 ,"house"),
		SegDataset(27 ,"sea"),
		SegDataset(28,"mirror"),
		SegDataset(29,"rug; carpet; carpeting"),
		SegDataset(30,"field"),
		SegDataset(31,"armchair"),
		SegDataset(32,"seat"),
		SegDataset(33,"fence; fencing"),
		SegDataset(34,"desk"),
		SegDataset(35,"rock; stone"),
		SegDataset(36,"wardrobe; closet; press"),
		SegDataset(37,"lamp"),
		SegDataset(38,"bathtub; bathing tub; bath; tub"),
		SegDataset(39,"railing; rail"),
		SegDataset(40,"cushion"),
		SegDataset(41,"base; pedestal; stand"),
		SegDataset(42,"box"),
		SegDataset(43,"column; pillar"),
		SegDataset(44,"signboard; sign"),
		SegDataset(45,"chest of drawers; chest; bureau; dresser"),
		SegDataset(46,"counter"),
		SegDataset(47,"sand"),
		SegDataset(48,"sink"),
		SegDataset(49,"skyscraper"),
		SegDataset(50,"fireplace; hearth; open fireplace"),
		SegDataset(51,"refrigerator; icebox"),
		SegDataset(52,"grandstand; covered stand"),
		SegDataset(53,"path"),
		SegDataset(54,"stairs; steps"),
		SegDataset(55,"runway"),
		SegDataset(56,"case; display case; showcase; vitrine"),
		SegDataset(57,"pool table; billiard table; snooker table"),
		SegDataset(58,"pillow"),
		SegDataset(59,"screen door; screen"),
		SegDataset(60,"stairway; staircase"),
		SegDataset(61,"river"),
		SegDataset(62,"bridge; span"),
		SegDataset(63,"bookcase"),
		SegDataset(64,"blind; screen"),
		SegDataset(65,"coffee table; cocktail table"),
		SegDataset(66,"toilet; can; commode; crapper; pot; potty; stool; throne"),
		SegDataset(67,"flower"),
		SegDataset(68,"book"),
		SegDataset(69,"hill"),
		SegDataset(70,"bench"),
		SegDataset(71,"countertop"),
		SegDataset(72,"stove; kitchen stove; range; kitchen range; cooking stove"),
		SegDataset(73,"palm; palm tree"),
		SegDataset(74,"kitchen island"),
		SegDataset(75,"computer; computing machine; computing device; data processor; electronic computer; information processing system"),
		SegDataset(76,"swivel chair"),
		SegDataset(77,"boat"),
		SegDataset(78,"bar"),
		SegDataset(79,"arcade machine"),
		SegDataset(80,"hovel; hut; hutch; shack; shanty"),
		SegDataset(81,"bus; autobus; coach; charabanc; double-decker; jitney; motorbus; motorcoach; omnibus; passenger vehicle"),
		SegDataset(82,"towel"),
		SegDataset(83,"light; light source"),
		SegDataset(84,"truck; motortruck"),
		SegDataset(85,"tower"),
		SegDataset(86,"chandelier; pendant; pendent"),
		SegDataset(87,"awning; sunshade; sunblind"),
		SegDataset(88,"streetlight; street lamp"),
		SegDataset(89,"booth; cubicle; stall; kiosk"),
		SegDataset(90,"television receiver; television; television set; tv; tv set; idiot box; boob tube; telly; goggle box"),
		SegDataset(91,"airplane; aeroplane; plane"),
		SegDataset(92,"dirt track"),
		SegDataset(93,"apparel; wearing apparel; dress; clothes"),
		SegDataset(94,"pole"),
		SegDataset(95,"land; ground; soil"),
		SegDataset(96,"bannister; banister; balustrade; balusters; handrail"),
		SegDataset(97,"escalator; moving staircase; moving stairway"),
		SegDataset(98,"ottoman; pouf; pouffe; puff; hassock"),
		SegDataset(99,"bottle"),
		SegDataset(100,"buffet; counter; sideboard"),
		SegDataset(101,"poster; posting; placard; notice; bill; card"),
		SegDataset(102,"stage"),
		SegDataset(103,"van"),
		SegDataset(104,"ship"),
		SegDataset(105,"fountain"),
		SegDataset(106,"conveyer belt; conveyor belt; conveyer; conveyor; transporter"),
		SegDataset(107,"canopy"),
		SegDataset(108,"washer; automatic washer; washing machine"),
		SegDataset(109,"plaything; toy"),
		SegDataset(110,"swimming pool; swimming bath; natatorium"),
		SegDataset(111,"stool"),
		SegDataset(112,"barrel; cask"),
		SegDataset(113,"basket; handbasket"),
		SegDataset(114,"waterfall; falls"),
		SegDataset(115,"tent; collapsible shelter"),
		SegDataset(116,"bag"),
		SegDataset(117,"minibike; motorbike"),
		SegDataset(118,"cradle"),
		SegDataset(119,"oven"),
		SegDataset(120,"ball"),
		SegDataset(121,"food; solid food"),
		SegDataset(122,"step; stair"),
		SegDataset(123,"tank; storage tank"),
		SegDataset(124,"trade name; brand name; brand; marque"),
		SegDataset(125,"microwave; microwave oven"),
		SegDataset(126,"pot; flowerpot"),
		SegDataset(127,"animal; animate being; beast; brute; creature; fauna"),
		SegDataset(128,"bicycle; bike; wheel; cycle "),
		SegDataset(129,"lake"),
		SegDataset(130,"dishwasher; dish washer; dishwashing machine"),
		SegDataset(131,"screen; silver screen; projection screen"),
		SegDataset(132,"blanket; cover"),
		SegDataset(133,"sculpture"),
		SegDataset(134,"hood; exhaust hood"),
		SegDataset(135,"sconce"),
		SegDataset(136,"vase"),
		SegDataset(137,"traffic light; traffic signal; stoplight"),
		SegDataset(138,"tray"),
		SegDataset(139,"ashcan; trash can; garbage can; wastebin; ash bin; ash-bin; ashbin; dustbin; trash barrel; trash bin"),
		SegDataset(140,"fan"),
		SegDataset(141,"pier; wharf; wharfage; dock"),
		SegDataset(142,"crt screen"),
		SegDataset(143,"plate"),
		SegDataset(144,"monitor; monitoring device"),
		SegDataset(145,"bulletin board; notice board"),
		SegDataset(146,"shower"),
		SegDataset(147,"radiator"),
		SegDataset(148,"glass; drinking glass"),
		SegDataset(149,"clock"),
		SegDataset(150,"flag")	
	};
	

// 	constexpr int class_count = 21; // background + 20 classes
// 
// 	const std::vector<SegDataset> classes = {
// 		SegDataset(0, ""), // background
// 
// 						   // The cream-colored `void' label is used in border regions and to mask difficult objects
// 						   // (see http://host.robots.ox.ac.uk/pascal/VOC/voc2012/htmldoc/devkit_doc.html)
// 						   SegDataset(dlib::loss_multiclass_log_per_pixel_::label_to_ignore,
// 						   "border"),
// 
// 						   SegDataset(1,  "aeroplane"),
// 						   SegDataset(2,  "bicycle"),
// 						   SegDataset(3,  "bird"),
// 						   SegDataset(4,  "boat"),
// 						   SegDataset(5,  "bottle"),
// 						   SegDataset(6,  "bus"),
// 						   SegDataset(7,  "car"),
// 						   SegDataset(8,  "cat"),
// 						   SegDataset(9,  "chair"),
// 						   SegDataset(10, "cow"),
// 						   SegDataset(11, "diningtable"),
// 						   SegDataset(12, "dog"),
// 						   SegDataset(13, "horse"),
// 						   SegDataset(14, "motorbike"),
// 						   SegDataset(15, "person"),
// 						   SegDataset(16, "pottedplant"),
// 						   SegDataset(17, "sheep"),
// 						   SegDataset(18, "sofa"),
// 						   SegDataset(19, "train"),
// 						   SegDataset(20, "tvmonitor"),
// 	};
		

// 	constexpr int class_count = 2; // background + 20 classes
//     const std::vector<SegDataset> classes = {
//         SegDataset(0, "skin"), // background
// 
//         // The cream-colored `void' label is used in border regions and to mask difficult objects
//         // (see http://host.robots.ox.ac.uk/pascal/VOC/voc2012/htmldoc/devkit_doc.html)
//         SegDataset(dlib::loss_multiclass_log_per_pixel_::label_to_ignore,
//              "border"),
// 
//         SegDataset(1,  "eye"),
//         SegDataset(2,  "white_eye"),
//     };
}



/*
template <typename Predicate>
const SegDataset& find_voc2012_class(Predicate predicate)
{
    const auto i = std::find_if(classes.begin(), classes.end(), predicate);

    if (i != classes.end())
    {
        return *i;
    }
    else
    {
        throw std::runtime_error("Unable to find a matching VOC2012 class");
    }
}*/

// ----------------------------------------------------------------------------------------

// Introduce the building blocks used to define the segmentation network.
// The network first does residual downsampling (similar to the dnn_imagenet_(train_)ex 
// example program), and then residual upsampling. The network could be improved e.g.
// by introducing skip connections from the input image, and/or the first layers, to the
// last layer(s).  (See Long et al., Fully Convolutional Networks for Semantic Segmentation,
// https://people.eecs.berkeley.edu/~jonlong/long_shelhamer_fcn.pdf)

template <int N, template <typename> class BN, int stride, typename SUBNET> 
using block = BN<dlib::con<N,3,3,1,1, dlib::relu<BN<dlib::con<N,3,3,stride,stride,SUBNET>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET> 
using blockt = BN<dlib::cont<N,3,3,1,1,dlib::relu<BN<dlib::cont<N,3,3,stride,stride,SUBNET>>>>>;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = dlib::add_prev1<block<N,BN,1,dlib::tag1<SUBNET>>>;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = dlib::add_prev2<dlib::avg_pool<2,2,2,2,dlib::skip1<dlib::tag2<block<N,BN,2,dlib::tag1<SUBNET>>>>>>;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_up = dlib::add_prev2<dlib::cont<N,2,2,2,2,dlib::skip1<dlib::tag2<blockt<N,BN,2,dlib::tag1<SUBNET>>>>>>;

template <int N, typename SUBNET> using res       = dlib::relu<residual<block,N,dlib::bn_con,SUBNET>>;
template <int N, typename SUBNET> using ares      = dlib::relu<residual<block,N,dlib::affine,SUBNET>>;
template <int N, typename SUBNET> using res_down  = dlib::relu<residual_down<block,N,dlib::bn_con,SUBNET>>;
template <int N, typename SUBNET> using ares_down = dlib::relu<residual_down<block,N,dlib::affine,SUBNET>>;
template <int N, typename SUBNET> using res_up    = dlib::relu<residual_up<block,N,dlib::bn_con,SUBNET>>;
template <int N, typename SUBNET> using ares_up   = dlib::relu<residual_up<block,N,dlib::affine,SUBNET>>;

// ----------------------------------------------------------------------------------------

template <typename SUBNET> using res512 = res<512, SUBNET>;
template <typename SUBNET> using res256 = res<256, SUBNET>;
template <typename SUBNET> using res128 = res<128, SUBNET>;
template <typename SUBNET> using res64  = res<64, SUBNET>;
template <typename SUBNET> using ares512 = ares<512, SUBNET>;
template <typename SUBNET> using ares256 = ares<256, SUBNET>;
template <typename SUBNET> using ares128 = ares<128, SUBNET>;
template <typename SUBNET> using ares64  = ares<64, SUBNET>;


template <typename SUBNET> using level1 = dlib::repeat<2,res512,res_down<512,SUBNET>>;
template <typename SUBNET> using level2 = dlib::repeat<2,res256,res_down<256,SUBNET>>;
template <typename SUBNET> using level3 = dlib::repeat<2,res128,res_down<128,SUBNET>>;
template <typename SUBNET> using level4 = dlib::repeat<2,res64,res<64,SUBNET>>;

template <typename SUBNET> using alevel1 = dlib::repeat<2,ares512,ares_down<512,SUBNET>>;
template <typename SUBNET> using alevel2 = dlib::repeat<2,ares256,ares_down<256,SUBNET>>;
template <typename SUBNET> using alevel3 = dlib::repeat<2,ares128,ares_down<128,SUBNET>>;
template <typename SUBNET> using alevel4 = dlib::repeat<2,ares64,ares<64,SUBNET>>;

template <typename SUBNET> using level1t = dlib::repeat<2,res512,res_up<512,SUBNET>>;
template <typename SUBNET> using level2t = dlib::repeat<2,res256,res_up<256,SUBNET>>;
template <typename SUBNET> using level3t = dlib::repeat<2,res128,res_up<128,SUBNET>>;
template <typename SUBNET> using level4t = dlib::repeat<2,res64,res_up<64,SUBNET>>;

template <typename SUBNET> using alevel1t = dlib::repeat<2,ares512,ares_up<512,SUBNET>>;
template <typename SUBNET> using alevel2t = dlib::repeat<2,ares256,ares_up<256,SUBNET>>;
template <typename SUBNET> using alevel3t = dlib::repeat<2,ares128,ares_up<128,SUBNET>>;
template <typename SUBNET> using alevel4t = dlib::repeat<2,ares64,ares_up<64,SUBNET>>;

// ----------------------------------------------------------------------------------------

// training network type
using net_type = dlib::loss_multiclass_log_per_pixel<
                            dlib::cont<class_count,7,7,2,2,
                            level4t<level3t<level2t<level1t<
                            level1<level2<level3<level4<
                            dlib::max_pool<3,3,2,2,dlib::relu<dlib::bn_con<dlib::con<64,7,7,2,2,
                            dlib::input<dlib::matrix<dlib::rgb_pixel>>
                            >>>>>>>>>>>>>>;

// testing network type (replaced batch normalization with fixed affine transforms)
using anet_type = dlib::loss_multiclass_log_per_pixel<
                            dlib::cont<class_count,7,7,2,2,
                            alevel4t<alevel3t<alevel2t<alevel1t<
                            alevel1<alevel2<alevel3<alevel4<
                            dlib::max_pool<3,3,2,2,dlib::relu<dlib::affine<dlib::con<64,7,7,2,2,
                            dlib::input<dlib::matrix<dlib::rgb_pixel>>
                            >>>>>>>>>>>>>>;

// ----------------------------------------------------------------------------------------

#endif // DLIB_DNn_SEMANTIC_SEGMENTATION_EX_H_
