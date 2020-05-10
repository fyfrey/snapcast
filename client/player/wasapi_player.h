/***
    This file is part of snapcast
    Copyright (C) 2014-2016  Johannes Pohl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***/

#ifndef WASAPI_PLAYER_H
#define WASAPI_PLAYER_H
#include "client_settings.hpp"
#include "player.hpp"
#include <audiopolicy.h>
#include <endpointvolume.h>

class AudioSessionEventListener : public IAudioSessionEvents
{
    LONG _cRef;

    float volume_ = 1.f;
    bool muted_ = false;

public:
    AudioSessionEventListener() : _cRef(1)
    {
    }

    float getVolume()
    {
        return volume_;
    }

    bool getMuted()
    {
        return muted_;
    }

    ~AudioSessionEventListener()
    {
    }

    // IUnknown methods -- AddRef, Release, and QueryInterface

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return InterlockedIncrement(&_cRef);
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        ULONG ulRef = InterlockedDecrement(&_cRef);
        if (0 == ulRef)
        {
            delete this;
        }
        return ulRef;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface);

    // Notification methods for audio session events

    HRESULT STDMETHODCALLTYPE OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID EventContext)
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID EventContext)
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(float NewVolume, BOOL NewMute, LPCGUID EventContext);

    HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged(DWORD ChannelCount, float NewChannelVolumeArray[], DWORD ChangedChannel, LPCGUID EventContext)
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(LPCGUID NewGroupingParam, LPCGUID EventContext)
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnStateChanged(AudioSessionState NewState);

    HRESULT STDMETHODCALLTYPE OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason);
};


class AudioEndpointVolumeCallback : public IAudioEndpointVolumeCallback
{
    LONG _cRef;
    float volume_ = 1.f;
    bool muted_ = false;

public:
    AudioEndpointVolumeCallback() : _cRef(1)
    {
    }

    ~AudioEndpointVolumeCallback()
    {
    }

    float getVolume()
    {
        return volume_;
    }

    bool getMuted()
    {
        return muted_;
    }

    // IUnknown methods -- AddRef, Release, and QueryInterface

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return InterlockedIncrement(&_cRef);
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        ULONG ulRef = InterlockedDecrement(&_cRef);
        if (0 == ulRef)
        {
            delete this;
        }
        return ulRef;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface)
    {
        if (IID_IUnknown == riid)
        {
            AddRef();
            *ppvInterface = (IUnknown*)this;
        }
        else if (__uuidof(IAudioEndpointVolumeCallback) == riid)
        {
            AddRef();
            *ppvInterface = (IAudioEndpointVolumeCallback*)this;
        }
        else
        {
            *ppvInterface = NULL;
            return E_NOINTERFACE;
        }

        return S_OK;
    }

    // Callback method for endpoint-volume-change notifications.

    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
};

class WASAPIPlayer : public Player
{
public:
    WASAPIPlayer(const PcmDevice& pcmDevice, std::shared_ptr<Stream> stream, ClientSettings::SharingMode mode);
    virtual ~WASAPIPlayer();

    static std::vector<PcmDevice> pcm_list(void);

protected:
    virtual void worker();

private:
    AudioSessionEventListener* audioEventListener_;
    IAudioEndpointVolume* audioEndpointListener_;
    ClientSettings::SharingMode mode_;
};

#endif
