#pragma once

class Frame
{
public:
    explicit Frame(float printIntervalSeconds = 0.5f);

    void Update(float dt);
    void SetEnabled(bool value);
    bool IsEnabled() const;

private:
    bool enabled = true;
    int frameCount = 0;
    float elapsedSeconds = 0.0f;
    float printIntervalSeconds = 0.5f;
};
