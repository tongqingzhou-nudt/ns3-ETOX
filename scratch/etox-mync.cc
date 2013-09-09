#include "ns3/etox-module.h"
#include "ns3/mync-module.h"
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4-global-routing-helper.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ETOXMyncTest");

void ReceivePacket (Ptr<Socket> socket)
{
	Ptr<Node> node = socket->GetNode();
	Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
	Ipv4Address addr = ipv4->GetAddress(ipv4->GetInterfaceForDevice(ipv4->GetNetDevice(1)), 0).GetLocal();

  NS_LOG_UNCOND("Received one packet at "<<socket<<addr<<"at "<<Simulator::Now().GetSeconds());
}

int main(int argc, char *argv[])
{
	LogComponentEnable("ETOXMyncTest", LOG_LEVEL_FUNCTION);
	LogComponentEnable("OnOffApplication", LOG_LEVEL_FUNCTION);
	LogComponentEnable("UdpL4Protocol", LOG_LEVEL_FUNCTION);
	LogComponentEnable("Ipv4L3Protocol", LOG_LEVEL_LOGIC);
	LogComponentEnable("UdpSocketImpl", LOG_LEVEL_LOGIC);
	LogComponentEnable("Socket", LOG_LEVEL_FUNCTION);
	LogComponentEnable("UdpSocket", LOG_LEVEL_FUNCTION);
	LogComponentEnable("MyncDevice", LOG_LEVEL_LOGIC);
	LogComponentEnable("MyncHeader", LOG_LEVEL_FUNCTION);
	LogComponentEnable("MyncPacketInfo", LOG_LEVEL_FUNCTION);
	LogComponentEnable("MyncProtocol", LOG_LEVEL_LOGIC);
	LogComponentEnable("MyncNeighbor", LOG_LEVEL_LOGIC);
	LogComponentEnable("MyncQueue", LOG_LEVEL_FUNCTION);
	LogComponentEnable("MyncCodeNum", LOG_LEVEL_FUNCTION);
	LogComponentEnable("WifiNetDevice", LOG_LEVEL_FUNCTION);
	LogComponentEnable("RegularWifiMac", LOG_LEVEL_FUNCTION);
	LogComponentEnable("AdhocWifiMac", LOG_LEVEL_FUNCTION);
	LogComponentEnable("DcaTxop", LOG_LEVEL_FUNCTION);
	LogComponentEnable("MacLow", LOG_LEVEL_FUNCTION);
	LogComponentEnable("YansWifiPhy", LOG_LEVEL_FUNCTION);
	LogComponentEnable("WifiPhy", LOG_LEVEL_FUNCTION);
	LogComponentEnable("YansWifiChannel", LOG_LEVEL_FUNCTION);
	LogComponentEnable("ETOXLinkStateRouting", LOG_LEVEL_LOGIC);
	LogComponentEnable("ETOXLinkStateNode", LOG_LEVEL_LOGIC);
	LogComponentEnable("ETOXLinkStateHeader", LOG_LEVEL_FUNCTION);
	LogComponentEnable("Dijkstra", LOG_LEVEL_LOGIC);
	LogComponentEnable("BinaryHeap", LOG_LEVEL_FUNCTION);

	std::string phyMode ("ErpOfdmRate24Mbps");
	double rss = -80;  // -dBm
	uint32_t packetSize = 1500; // bytes
	uint32_t numPackets = 1;
	bool verbose = false;
	Ipv4Address net("10.1.1.0");
	Ipv4Address base("0.0.0.1");
	Ipv4Mask mask("255.255.255.0");
	std::string dataRate("6Mbps");
	double rtTime = 20;
	double tryTime = 5000;
	bool mr = true;
	uint32_t nodeNum = 3;

	MyncHelper mync;

	CommandLine cmd;
	cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
	cmd.AddValue ("rss", "received signal strength", rss);
	cmd.AddValue ("packetSize", "size of application packet sent", packetSize);
	cmd.AddValue ("numPackets", "number of packets generated", numPackets);
	cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
	cmd.AddValue ("dataRate", "data rate of OnOff app", dataRate);
	cmd.AddValue ("rtTime", "retransmission time of Cope", rtTime);
	cmd.AddValue ("tryTime", "for TrySend timer", tryTime);
	cmd.AddValue ("mr", "whether to use multi-radio", mr);
	cmd.AddValue ("nodeNum", "Number of nodes", nodeNum) ;
	cmd.Parse(argc, argv);

	Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue("2200"));
	Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("1"));
	Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (phyMode));

	NodeContainer node_container;
	node_container.Create(nodeNum);

	WifiHelper wifi;
	if (verbose)
	{
		wifi.EnableLogComponents ();  // Turn on all Wifi logging
	}

	wifi.SetStandard(WIFI_PHY_STANDARD_80211g);

	YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
	wifiPhy.Set("RxGain", DoubleValue(0));
	wifiPhy.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
	Ptr<YansWifiChannel> channel = wifiChannel.Create();
	wifiPhy.SetChannel(channel);

	NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default();
	wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue(phyMode), "ControlMode", StringValue(phyMode), "NonUnicastMode", StringValue(phyMode));
	wifiMac.SetType("ns3::AdhocWifiMac");

	NetDeviceContainer devices = wifi.Install(wifiPhy, wifiMac, node_container);

	/*--- Set up another group of radios --*/
	/*
	std::string phyMode2 = "ErpOfdmRate24Mbps"; //"OfdmRate54Mbps";

	Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (phyMode2));

	WifiHelper wifi2;
	if (verbose)
	{
		wifi2.EnableLogComponents ();  // Turn on all Wifi logging
	}

	wifi2.SetStandard(WIFI_PHY_STANDARD_80211g);

	YansWifiPhyHelper wifiPhy2 = YansWifiPhyHelper::Default();
	wifiPhy2.Set("RxGain", DoubleValue(0));
	wifiPhy2.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
	wifiPhy2.Set("ChannelNumber", UintegerValue(11));

	wifiPhy2.SetChannel(channel);

	wifi2.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue(phyMode2), "ControlMode", StringValue(phyMode2), "NonUnicastMode", StringValue(phyMode2));

	devices.Add(wifi2.Install(wifiPhy2, wifiMac, node_container));
	*/

	NetDeviceContainer myncDevices = mync.InstallProtocol(node_container, mr, rtTime, tryTime, 120);

	MobilityHelper mobility;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
	for(uint32_t i = 0; i<nodeNum; i++)
		positionAlloc->Add (Vector (0.0, 300*i, 0.0));
	mobility.SetPositionAllocator(positionAlloc);
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.Install(node_container);
	NS_LOG_UNCOND("should be here");

	ETOXLinkStateHelper etoxLS(25, nodeNum-1);
	Ipv4ListRoutingHelper list;
	list.Add(etoxLS, 10);

	InternetStackHelper internet;
	internet.SetRoutingHelper(list);
	internet.Install(node_container);

	//Set up IPv4 for Mync and start MyncProtocol
	Ipv4AddressHelper ipv4;
	ipv4.SetBase(net, mask, base);
	Ipv4InterfaceContainer i = ipv4.Assign(myncDevices);
	for(uint32_t i = 0; i<myncDevices.GetN(); i++)
	{
		Ptr<mync::MyncDevice> myncDevice = myncDevices.Get(i)->GetObject<mync::MyncDevice>();
		myncDevice->SetUpIp();
	}
	mync.StartProtocol();

	Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("etox160.routes", std::ios::out);
	etoxLS.PrintRoutingTableAllAt (Seconds(160.0), routingStream);
	Ptr<OutputStreamWrapper> routingStream2 = Create<OutputStreamWrapper> ("etox140.routes", std::ios::out);
	etoxLS.PrintRoutingTableAllAt (Seconds(140.0), routingStream2);
	Ptr<OutputStreamWrapper> routingStream3 = Create<OutputStreamWrapper> ("etox230.routes", std::ios::out);
	etoxLS.PrintRoutingTableAllAt (Seconds(230.0), routingStream3);

	Ptr<mync::MyncProtocol> myncProtocol_oo = node_container.Get(0)->GetObject<mync::MyncProtocol>();
	Address remoteAddress(InetSocketAddress(myncProtocol_oo->GetIP(0), 80));
	OnOffHelper oohelper("ns3::UdpSocketFactory", remoteAddress);
	oohelper.SetAttribute("DataRate", StringValue(dataRate));
	oohelper.SetAttribute("MaxBytes", UintegerValue(packetSize*3000));
	oohelper.SetAttribute("PacketSize", UintegerValue(packetSize));
	ApplicationContainer apps = oohelper.Install(node_container.Get(nodeNum-1));
	apps.Start(Seconds(140.0));
	apps.Stop(Seconds(150.0));

	Ptr<mync::MyncProtocol> myncProtocol_oo2 = node_container.Get(nodeNum-1)->GetObject<mync::MyncProtocol>();
	Address remoteAddress2(InetSocketAddress(myncProtocol_oo2->GetIP(0), 80));
	OnOffHelper oohelper2("ns3::UdpSocketFactory", remoteAddress2);
	oohelper2.SetAttribute("DataRate", StringValue(dataRate));
	oohelper2.SetAttribute("PacketSize", UintegerValue(packetSize));
	oohelper2.SetAttribute("MaxBytes", UintegerValue(packetSize*3000));
	ApplicationContainer apps2 = oohelper2.Install(node_container.Get(0));
	apps2.Start(Seconds(141.0));
	apps2.Stop(Seconds(151.0));

	TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
	Ptr<Socket> recvSink = Socket::CreateSocket (node_container.Get(0), tid);
	Ptr<Socket> recvSink2 = Socket::CreateSocket (node_container.Get(nodeNum-1), tid);
	
	recvSink->SetAttribute("RcvBufSize", UintegerValue(1310720*2));
	recvSink2->SetAttribute("RcvBufSize", UintegerValue(1310720*2));

	InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 80);
	InetSocketAddress local2 = InetSocketAddress(Ipv4Address::GetAny(), 80);

	if(recvSink->Bind(local) == -1)
		NS_LOG_ERROR("Bind failure of node 0");
	if (recvSink2->Bind(local2) == -1)
		NS_LOG_ERROR("Bind failure of node 2");

	recvSink->SetRecvCallback (MakeCallback(&ReceivePacket));
	recvSink2->SetRecvCallback (MakeCallback(&ReceivePacket));

	AsciiTraceHelper ascii;
	std::string ascfile ("mrmync-24");
	std::string ascfile_post (".tr");
	std::string ascfile_middle;
	std::stringstream out;
	out<<"-"<<nodeNum<<"-"<<dataRate<<"-"<<packetSize;
	ascfile_middle = out.str();
	
	wifiPhy.EnableAsciiAll(ascii.CreateFileStream(ascfile+ascfile_middle+ascfile_post));

	Simulator::Stop(Seconds(230.0));
	Simulator::Run();
	Simulator::Destroy();

	return 0;
}
