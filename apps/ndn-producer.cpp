/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "ndn-producer.hpp"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/double.h"

#include "model/ndn-app-face.hpp"
#include "model/ndn-ns3.hpp"
#include "model/ndn-l3-protocol.hpp"
#include "helper/ndn-fib-helper.hpp"

#include <cryptopp/asn.h>
#include <cryptopp/base64.h>
#include <cryptopp/des.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/pssr.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/rsa.h>
#include <cryptopp/sha.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/oids.h>
#include <cryptopp/dsa.h>

#include <memory>

NS_LOG_COMPONENT_DEFINE("ndn.Producer");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(Producer);

std::string
SHA256HashString(std::string aString) {
    std::string digest;
    CryptoPP::SHA256 hash;

    CryptoPP::StringSource foo(aString, true,
    new CryptoPP::HashFilter(hash,
      new CryptoPP::Base64Encoder (
         new CryptoPP::StringSink(digest))));

    return digest;
}

TypeId
Producer::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::Producer")
      .SetGroupName("Ndn")
      .SetParent<App>()
      .AddConstructor<Producer>()
      .AddAttribute("Prefix", "Prefix, for which producer has the data", StringValue("/"),
                    MakeNameAccessor(&Producer::m_prefix), MakeNameChecker())

      .AddAttribute(
         "Postfix",
         "Postfix that is added to the output data (e.g., for adding producer-uniqueness)",
         StringValue("/"), MakeNameAccessor(&Producer::m_postfix), MakeNameChecker())

      .AddAttribute("PayloadSize", "Virtual payload size for Content packets", UintegerValue(1024),
                    MakeUintegerAccessor(&Producer::m_virtualPayloadSize),
                    MakeUintegerChecker<uint32_t>())

      .AddAttribute("Freshness", "Freshness of data packets, if 0, then unlimited freshness",
                    TimeValue(Seconds(0)), MakeTimeAccessor(&Producer::m_freshness),
                    MakeTimeChecker())

      .AddAttribute("Frequency", "Frequency of interest packets", StringValue("1.0"),
                  MakeDoubleAccessor(&Producer::m_frequency), MakeDoubleChecker<double>())

      .AddAttribute("Randomize",
                  "Type of send time randomization: none (default), uniform, exponential",
                  StringValue("none"),
                  MakeStringAccessor(&Producer::SetRandomize, &Producer::GetRandomize),
                  MakeStringChecker())

      .AddAttribute(
         "Signature",
         "Fake signature, 0 valid signature (default), other values application-specific",
         UintegerValue(0), MakeUintegerAccessor(&Producer::m_signature),
         MakeUintegerChecker<uint32_t>())
      .AddAttribute("KeyLocator",
                    "Name to be used for key locator.  If root, then key locator is not used",
                    NameValue(), MakeNameAccessor(&Producer::m_keyLocator), MakeNameChecker());
  return tid;
}

Producer::Producer() : m_frequency(1.0), m_firstTime(true)
{
  NS_LOG_FUNCTION_NOARGS();
}

// inherited from Application base class.
void
Producer::StartApplication()
{
  NS_LOG_FUNCTION_NOARGS();
  App::StartApplication();

  FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);

  ScheduleNextBead();
}

void
Producer::StopApplication()
{
  NS_LOG_FUNCTION_NOARGS();

  App::StopApplication();
}

void
Producer::SetRandomize(std::string value)
{
    if (value == "uniform") {
      m_random = CreateObject<UniformRandomVariable>();
      m_random->SetAttribute("Min", DoubleValue(0.0));
      m_random->SetAttribute("Max", DoubleValue(2 * 1.0 / m_frequency));
    }
    else if (value == "exponential") {
      m_random = CreateObject<ExponentialRandomVariable>();
      m_random->SetAttribute("Mean", DoubleValue(1.0 / m_frequency));
      m_random->SetAttribute("Bound", DoubleValue(50 * 1.0 / m_frequency));
    }
    else
      m_random = 0;

    m_randomType = value;
}

std::string
Producer::GetRandomize() const
{
  return m_randomType;
}

void
Producer::ScheduleNextBead()
{
  // double mean = 8.0 * m_payloadSize / m_desiredRate.GetBitRate ();
  std::cout << "next called: " << Simulator::Now().ToDouble(Time::S) << "s\n";

  if (m_firstTime) {
    m_sendEvent = Simulator::Schedule(Seconds(0.0), &Producer::SendBead, this);
    m_firstTime = false;
  }
  else if (!m_sendEvent.IsRunning()) {
    m_sendEvent = Simulator::Schedule((m_random == 0) ? Seconds(1.0 / m_frequency)
                                                      : Seconds(m_random->GetValue()),
                                      &Producer::SendBead, this);
  }
}

void
Producer::SendBead()
{
    std::cout << "Enter SendBead" << std::endl;

    if (dataNames.size() > 0) {
        std::string name = dataNames.back();
        std::string preimage = preimages.back();

        Name dataName(name);

        auto bead = make_shared<Bead>();
        bead->setName(dataName);

        bead->setToken(preimage);

        // bead->setFreshnessPeriod(::ndn::time::milliseconds(m_freshness.GetMilliSeconds()));
        // bead->setContent(make_shared<::ndn::Buffer>(m_virtualPayloadSize));

        // Signature signature;
        // SignatureInfo signatureInfo(static_cast< ::ndn::tlv::SignatureTypeValue>(255));
        // if (m_keyLocator.size() > 0) {
        //   signatureInfo.setKeyLocator(m_keyLocator);
        // }
        // signature.setInfo(signatureInfo);
        // signature.setValue(::ndn::nonNegativeIntegerBlock(::ndn::tlv::SignatureValue, m_signature));
        // bead->setSignature(signature);

        NS_LOG_INFO("node(" << GetNode()->GetId() << ") responding with BEAD: " << bead->getName());

        // to create real wire encoding
        bead->wireEncode();

        m_transmittedBeads(bead, this, m_face);
        m_face->onReceiveBead(*bead);

    }
}

void
Producer::OnInterest(shared_ptr<const Interest> interest)
{
  App::OnInterest(interest); // tracing inside

  NS_LOG_FUNCTION(this << interest);

  if (!m_active)
    return;

  Name dataName(interest->getName());
  // dataName.append(m_postfix);
  // dataName.appendVersion();

  auto data = make_shared<Data>();
  data->setName(dataName);

  // Ensure that data is saved so we can send BEADs later
  // datas.push_back(data);
  dataNames.push_back(dataName.toUri());

  data->setFreshnessPeriod(::ndn::time::milliseconds(m_freshness.GetMilliSeconds()));

  data->setContent(make_shared< ::ndn::Buffer>(m_virtualPayloadSize));

  CryptoPP::AutoSeededRandomPool rng;

  uint32_t preimageInt = rng.GenerateWord32();
  std::ostringstream str;
  str << preimageInt;
  std::string preimage = str.str();
  preimages.push_back(preimage);


  std::string image = SHA256HashString(preimage);
  data->setToken(image);

  Signature signature;
  SignatureInfo signatureInfo(static_cast< ::ndn::tlv::SignatureTypeValue>(255));

  if (m_keyLocator.size() > 0) {
    signatureInfo.setKeyLocator(m_keyLocator);
  }

  signature.setInfo(signatureInfo);
  signature.setValue(::ndn::nonNegativeIntegerBlock(::ndn::tlv::SignatureValue, m_signature));

  data->setSignature(signature);

  NS_LOG_INFO("node(" << GetNode()->GetId() << ") responding with Data: " << data->getName());

  // to create real wire encoding
  data->wireEncode();

  m_transmittedDatas(data, this, m_face);
  m_face->onReceiveData(*data);
  ScheduleNextBead();
}

} // namespace ndn
} // namespace ns3
