// SPDX-License-Identifier: GPLv3-or-later WITH Appstore-exception

#include "SonobusPluginProcessor.h"

#include <iostream>
#include <memory>

int main(int argc, char ** argv)
{
    if (argc != 3)
    {
        std::cerr << "usage: SonoBusAutoReconnectTest <server-host> <server-port>\n";
        return 2;
    }

    const juce::String serverHost(argv[1]);
    const int serverPort = juce::String(argv[2]).getIntValue();
    if (serverHost.isEmpty() || serverPort <= 0)
    {
        std::cerr << "invalid server address\n";
        return 2;
    }

    juce::ScopedJuceInitialiser_GUI juceInitialiser;
    juce::MemoryBlock savedState;
    const auto suffix = juce::String(juce::Time::getCurrentTime().toMilliseconds());
    const juce::String group("auto-reconnect-" + suffix);
    const juce::String user("auto-reconnect-" + suffix);

    {
        juce::AudioProcessor::setTypeOfNextNewPlugin(juce::AudioProcessor::wrapperType_AAX);
        auto source = std::make_unique<SonobusAudioProcessor>();

        AooServerConnectionInfo recent;
        recent.serverHost = serverHost;
        recent.serverPort = serverPort;
        recent.groupName = group;
        recent.userName = user;
        recent.timestamp = juce::Time::getCurrentTime().toMilliseconds();
        source->addRecentServerConnectionInfo(recent);

        auto * reconnect = source->getValueTreeState().getParameter(SonobusAudioProcessor::paramAutoReconnectLast);
        if (reconnect == nullptr)
        {
            std::cerr << "missing reconnectlast parameter\n";
            return 2;
        }

        reconnect->setValueNotifyingHost(1.0f);
        source->getStateInformation(savedState);
    }

    juce::AudioProcessor::setTypeOfNextNewPlugin(juce::AudioProcessor::wrapperType_AAX);
    auto restored = std::make_unique<SonobusAudioProcessor>();
    restored->setStateInformation(savedState.getData(), (int) savedState.getSize());

    for (int attempt = 0; attempt < 400; ++attempt)
    {
        if (restored->getCurrentJoinedGroup() == group)
        {
            std::cout << "restored and joined " << group << "\n";
            return 0;
        }
        juce::Thread::sleep(25);
    }

    std::cerr << "auto reconnect stopped at connected=" << (int) restored->isConnectedToServer()
              << " group='" << restored->getCurrentJoinedGroup() << "'\n";
    return 1;
}
