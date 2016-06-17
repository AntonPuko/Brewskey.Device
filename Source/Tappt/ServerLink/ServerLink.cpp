#include "ServerLink.h"

ServerLink::ServerLink(IStateManager *stateManager) {
  this->stateManager = stateManager;

  String deviceID = System.deviceID();

  Particle.subscribe(
		"hook-response/tappt_initialize-" + deviceID,
		&ServerLink::Initialize,
		this,
		MY_DEVICES
	);

	// Called by server when device settings are updated.
	Particle.function("settings", &ServerLink::Settings, this);

	// Called by server when user tries to pour.
	Particle.function("pour", &ServerLink::Pour, this);
	// Response when token is used
	Particle.subscribe(
		"hook-response/tappt_request-pour-" + deviceID,
		&ServerLink::PourResponse,
		this,
		MY_DEVICES
	);

  this->CallInitialize();
}

void ServerLink::CallInitialize() {
  this->initializeTimer.start();
  Serial.println("Getting device settings");
  Particle.publish("tappt_initialize", (const char *)0, 10, PRIVATE);
}

void ServerLink::Initialize(const char* event, const char* data) {
  if (strlen(data) <= 0) {
    return;
  }

  this->initializeTimer.stop();
  this->initializeTimer.dispose();

  char strBuffer[90] = "";
  String(data).toCharArray(strBuffer, 90);

  this->settings.deviceId = String(strtok(strBuffer, "~"));
  this->settings.authorizationToken = String(strtok((char*)NULL, "~"));
  this->settings.tapIds = String(strtok((char*)NULL, "~"));
  this->settings.deviceStatus = String(strtok((char*)NULL, "~"));

  Serial.print("Tap IDs: ");
  Serial.println(this->settings.tapIds);

  Serial.print("Device Status: ");
  Serial.println(this->settings.deviceStatus);

  this->stateManager->Initialize(&this->settings);
}

int ServerLink::Settings(String data) {
  this->CallInitialize();

	return 0;
}

void ServerLink::AuthorizePour(String deviceId, String authenticationKey) {
    Serial.println(authenticationKey);
    Serial.println("printed");

    sprintf(
      json,
      "{\"authToken\":\"%s\",\"id\":\"%s\",\"tkn\":\"%s\"}",
      this->settings.authorizationToken.c_str(),
      deviceId.c_str(),
      // remove \u0002 and "en"
      authenticationKey.substring(3).c_str()
    );

    Serial.print("Request Pour");Serial.println(json);
    Particle.publish("tappt_request-pour", json, 5, PRIVATE);
}

int ServerLink::Pour(String data) {
	Serial.print("Pour: ");
	Serial.println(data);

	if (data.length() <= 0) {
		return -1;
	}

  this->stateManager->StartPour(data);

	return 0;
}

void ServerLink::PourResponse(const char* event, const char* data) {
	this->Pour(String(data));
}

void ServerLink::SendPourToServer(
  String tapId,
  uint totalPulses,
  String authenticationKey
) {
  sprintf(
    json,
    "{\"authToken\":\"%s\",\"tapId\":\"%s\",\"pourKey\":\"%s\",\"pulses\":\"%d\"}",
    this->settings.authorizationToken.c_str(),
    tapId.c_str(),
    authenticationKey.c_str(),
    totalPulses
  );


  Serial.print("Finished Pour");Serial.println(json);
  Particle.publish("tappt_pour-finished", json, 60, PRIVATE);
}