#include "Debug.h"

#include "AssetLibrary.h"

using namespace NCL;

std::vector<Debug::DebugStringEntry> Debug::stringEntries[MAX_ENTRIES];
std::vector<Debug::DebugLineEntry>   Debug::lineEntries;

SimpleFont* Debug::debugFont = nullptr;

const Vector4 Debug::RED   = Vector4(1, 0, 0, 1);
const Vector4 Debug::GREEN = Vector4(0, 1, 0, 1);
const Vector4 Debug::BLUE  = Vector4(0, 0, 1, 1);

const Vector4 Debug::BLACK = Vector4(0, 0, 0, 1);
const Vector4 Debug::WHITE = Vector4(1, 1, 1, 1);

const Vector4 Debug::YELLOW  = Vector4(1, 1, 0, 1);
const Vector4 Debug::MAGENTA = Vector4(1, 0, 1, 1);
const Vector4 Debug::CYAN    = Vector4(0, 1, 1, 1);

void Debug::Print(const std::string& text, const Vector2& pos, const Vector4& colour, const float size, int player) {
	DebugStringEntry newEntry;

	newEntry.data = text;
	newEntry.position = pos;
	newEntry.colour = colour;
	newEntry.size = size;

	stringEntries[player].emplace_back(newEntry);
}

void Debug::DrawLine(const Vector3& startpoint, const Vector3& endpoint, const Vector4& colour, float time) {
	DebugLineEntry newEntry;

	newEntry.start = startpoint;
	newEntry.end = endpoint;
	newEntry.colourA = colour;
	newEntry.colourB = colour;
	newEntry.time = time;

	lineEntries.emplace_back(newEntry);
}

void Debug::DrawAxisLines(const Matrix4& modelMatrix, float scaleBoost, float time) {
	Matrix4 local = modelMatrix;
	local.SetPositionVector({ 0, 0, 0 });

	Vector3 fwd = local * Vector4(0, 0, -1, 1.0f);
	Vector3 up = local * Vector4(0, 1, 0, 1.0f);
	Vector3 right = local * Vector4(1, 0, 0, 1.0f);

	Vector3 worldPos = modelMatrix.GetPositionVector();

	DrawLine(worldPos, worldPos + (right * scaleBoost), Debug::RED, time);
	DrawLine(worldPos, worldPos + (up * scaleBoost), Debug::GREEN, time);
	DrawLine(worldPos, worldPos + (fwd * scaleBoost), Debug::BLUE, time);
}

void Debug::UpdateRenderables(float dt) {
	lineEntries.erase(std::remove_if(lineEntries.begin(), lineEntries.end(),
		[dt](DebugLineEntry& entry) {
			entry.time -= dt;
			return entry.time < 0;
		}),
		lineEntries.end());
	for (auto& entries : stringEntries) {
		entries.clear();
	}
}

SimpleFont* Debug::GetDebugFont() {
	if (!debugFont) {
		debugFont = new SimpleFont("PressStart2P.fnt", *AssetLibrary<TextureBase>::GetAsset("fontAtlas"));
	}
	return debugFont;
}

const std::vector<Debug::DebugStringEntry>& Debug::GetDebugStrings(int player) {
	return stringEntries[player];
}

const std::vector<Debug::DebugLineEntry>& Debug::GetDebugLines() {
	return lineEntries;
}