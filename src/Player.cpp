#include "Player.h"
#include "Config.h"
#include "Projectile.h"
#include <SFML/Graphics.hpp>
#include <stdio.h>

Player::Player(const char* texturePath, const char* weaponTexturePath, std::vector<Explosion*>* explosions) : Character(texturePath, weaponTexturePath, explosions, 3), m_pCoverSprite(new sf::Sprite), m_pCoverTex(new sf::Texture)
{
	m_pKeyFrameSize->x = 26;
	m_pKeyFrameSize->y = 25;
	m_pSpriteSheetSize->x = 7;
	m_pSpriteSheetSize->y = 2;
	m_BaseKeyFrame = 0;
	m_AnimationLength = 4;
	m_AnimationStart = 0;
	m_pCurrentKeyFrame->x = 5;
	m_pCurrentKeyFrame->y = 0;
	m_pStartPosition->x = Config::GetInstance().GetWindowSizeWidth() / 2;
	m_pStartPosition->y = 0;
	m_AnimationSpeed = 0.15f;
	m_KeyFrameDuration = 0.15f;

	m_BoundOffsetX = 8;
	m_BoundOffsetY = 1;

	m_pSprite->setTextureRect(sf::IntRect(m_pCurrentKeyFrame->x * m_pKeyFrameSize->x, m_pCurrentKeyFrame->y * m_pKeyFrameSize->y, m_pKeyFrameSize->x, m_pKeyFrameSize->y));
	SetPosition(*m_pStartPosition);

	m_WalkingSpeed = 0;
	m_SpeedX = 0;
	m_SpeedY = 0;

	m_ShowWeapon = false;
	m_pWeaponSprite->setTextureRect(sf::IntRect(m_pCurrentKeyFrame->x * m_pKeyFrameSize->x, m_pCurrentKeyFrame->y * m_pKeyFrameSize->y, m_pKeyFrameSize->x, m_pKeyFrameSize->y));
	m_pWeaponSprite->setOrigin(sf::Vector2f(m_pKeyFrameSize->x / 2, 18));
	m_pWeaponSprite->setPosition(sf::Vector2f(m_pStartPosition->x + m_pKeyFrameSize->x / 2 * SCALE, m_pStartPosition->y + 18 * SCALE));
	m_pWeaponSprite->setScale(SCALE, SCALE);
	m_WeaponAngle = 0;

	m_pCoverTex->loadFromFile("..\\resources\\cape.png");
	m_pCoverSprite->setTexture(*m_pCoverTex);
	m_pCoverSprite->setTextureRect(sf::IntRect(m_pCurrentKeyFrame->x * m_pKeyFrameSize->x, m_pCurrentKeyFrame->y * m_pKeyFrameSize->y, m_pKeyFrameSize->x, m_pKeyFrameSize->y));
	m_pCoverSprite->setPosition(*m_pStartPosition);
	m_pCoverSprite->setScale(SCALE, SCALE);

	m_Jumping = false;

	UpdateBounds();

	m_Weight = 2;

	m_IsHurt = false;

	m_Projectiles.push(new Projectile("..\\resources\\projectile.png", m_pExplosions, 50 * SCALE, 300 * SCALE));
}

Player::~Player()
{
	delete m_pCoverTex;
	delete m_pCoverSprite;
}

void Player::SetWalkingSpeed(int speed)
{
	m_WalkingSpeed = speed;
	m_IsHurt = false;
	if (!IsInAir())
	{
		m_SpeedX = m_WalkingSpeed;
	}
	else
	{
		m_SpeedX = m_WalkingSpeed / 2;
	}
}

void Player::ShowWeapon(bool show)
{
	m_ShowWeapon = show;
	if (m_ShowWeapon)
	{
		m_BaseKeyFrame = 2;
		m_AnimationLength = 2;
		m_AnimationStart = 1;

		m_IsHurt = false;
	}
	else
	{
		m_BaseKeyFrame = 0;
		m_AnimationLength = 4;
		m_AnimationStart = 0;
	}
}

void Player::Jump()
{
	if (!IsInAir())
	{
		m_SpeedY = -125 * SCALE;
		ShowWeapon(false);
		m_WalkingSpeed = 50 * SCALE * -(2 * m_pCurrentKeyFrame->y - 1) * (m_WalkingSpeed != 0);
		m_Jumping = true;
		m_IsHurt = false;
	}
}

bool Player::IsWeaponOut()
{
	return m_ShowWeapon;
}

void Player::Update(float dt, sf::RenderWindow * window, float offset)
{
	if (IsInAir())
	{
		ShowWeapon(false);
	}
	if (m_WalkingSpeed > 0 && !m_Exploded)
	{
		if (m_pCurrentKeyFrame->y == 1)
		{
			m_pWeaponSprite->setRotation(m_WeaponAngle);
			if (!m_Projectiles.front()->IsShooting())
			{
				m_Projectiles.front()->SetDirection(1);
			}
		}
		m_pCurrentKeyFrame->y = 0;
	}
	else if (m_WalkingSpeed < 0 && !m_Exploded)
	{
		if (m_pCurrentKeyFrame->y == 0)
		{
			m_pWeaponSprite->setRotation(360 - m_WeaponAngle);
			if (!m_Projectiles.front()->IsShooting())
			{
				m_Projectiles.front()->SetDirection(-1);
			}
		}
		m_pCurrentKeyFrame->y = 1;
	}

	Move(dt);
	m_pCoverSprite->setPosition(m_pSprite->getPosition());

	if (!m_Projectiles.front()->IsShooting())
	{
		m_Projectiles.front()->SetPosition(m_pWeaponSprite->getPosition().x, m_pWeaponSprite->getPosition().y - 5 * SCALE);
		m_Projectiles.front()->SetDirection(-(m_pCurrentKeyFrame->y * 2 - 1));
	}
	else
	{
		m_Projectiles.front()->Update(dt, window, offset);
	}

	if (m_WalkingSpeed != 0)
	{
		m_KeyFrameDuration += dt;
		if (m_KeyFrameDuration > m_AnimationSpeed)
		{
			m_pCurrentKeyFrame->x = ((m_pCurrentKeyFrame->x - m_AnimationStart) + 1) % m_AnimationLength + m_AnimationStart;
			m_pSprite->setTextureRect(sf::IntRect(m_pCurrentKeyFrame->x * m_pKeyFrameSize->x, m_pCurrentKeyFrame->y * m_pKeyFrameSize->y, m_pKeyFrameSize->x, m_pKeyFrameSize->y));
			m_pWeaponSprite->setTextureRect(sf::IntRect(m_pCurrentKeyFrame->x * m_pKeyFrameSize->x, m_pCurrentKeyFrame->y * m_pKeyFrameSize->y, m_pKeyFrameSize->x, m_pKeyFrameSize->y));
			m_pCoverSprite->setTextureRect(sf::IntRect(m_pCurrentKeyFrame->x * m_pKeyFrameSize->x, m_pCurrentKeyFrame->y * m_pKeyFrameSize->y, m_pKeyFrameSize->x, m_pKeyFrameSize->y));

			m_KeyFrameDuration = 0;
		}
	}
	else if (m_Exploded)
	{
		m_ShowWeapon = false;
		m_IsHurt = true;
		m_pSprite->setTextureRect(sf::IntRect(6 * m_pKeyFrameSize->x, (m_SpeedX < 0) * m_pKeyFrameSize->y, m_pKeyFrameSize->x, m_pKeyFrameSize->y));
		m_pCurrentKeyFrame->y = (m_SpeedX < 0);
		m_WalkingSpeed = 0;
		m_WeaponAngle = 0;
		if (m_pCurrentKeyFrame->y == 0)
		{
			m_pWeaponSprite->setRotation(m_WeaponAngle);
			if (!m_Projectiles.front()->IsShooting())
			{
				m_Projectiles.front()->SetDirection(1);
			}
		}
		if (m_pCurrentKeyFrame->y == 1)
		{
			m_pWeaponSprite->setRotation(360 - m_WeaponAngle);
			if (!m_Projectiles.front()->IsShooting())
			{
				m_Projectiles.front()->SetDirection(-1);
			}
		}
	}
	else if (m_IsHurt)
	{
		m_pSprite->setTextureRect(sf::IntRect(6 * m_pKeyFrameSize->x, m_pCurrentKeyFrame->y * m_pKeyFrameSize->y, m_pKeyFrameSize->x, m_pKeyFrameSize->y));
	}
	else if (m_SpeedY == 0 && m_pCurrentKeyFrame->x != 5)
	{
		m_pCurrentKeyFrame->x = m_BaseKeyFrame;
		m_pSprite->setTextureRect(sf::IntRect(m_pCurrentKeyFrame->x * m_pKeyFrameSize->x, m_pCurrentKeyFrame->y * m_pKeyFrameSize->y, m_pKeyFrameSize->x, m_pKeyFrameSize->y));
		m_pWeaponSprite->setTextureRect(sf::IntRect(m_pCurrentKeyFrame->x * m_pKeyFrameSize->x, m_pCurrentKeyFrame->y * m_pKeyFrameSize->y, m_pKeyFrameSize->x, m_pKeyFrameSize->y));
		m_pCoverSprite->setTextureRect(sf::IntRect(m_pCurrentKeyFrame->x * m_pKeyFrameSize->x, m_pCurrentKeyFrame->y * m_pKeyFrameSize->y, m_pKeyFrameSize->x, m_pKeyFrameSize->y));

		m_KeyFrameDuration = 0.15f;
	}
	else if (m_pCurrentKeyFrame->x != 5)
	{
		m_pSprite->setTextureRect(sf::IntRect(4 * m_pKeyFrameSize->x, m_pCurrentKeyFrame->y * m_pKeyFrameSize->y, m_pKeyFrameSize->x, m_pKeyFrameSize->y));
		m_pWeaponSprite->setTextureRect(sf::IntRect(0 * m_pKeyFrameSize->x, m_pCurrentKeyFrame->y * m_pKeyFrameSize->y, m_pKeyFrameSize->x, m_pKeyFrameSize->y));
		m_pCoverSprite->setTextureRect(sf::IntRect(0 * m_pKeyFrameSize->x, m_pCurrentKeyFrame->y * m_pKeyFrameSize->y, m_pKeyFrameSize->x, m_pKeyFrameSize->y));
	}

	if (m_LeftBound < (int)(offset - (Config::GetInstance().GetWindowSizeWidth() / 2)) + m_OffsetBounds)
	{
		m_SpeedX *= -0.25 * SCALE;
		Move(dt);
		if (abs(m_WalkingSpeed) > 0)
		{
			m_pSprite->move(20 * SCALE * dt, 0);
			m_pWeaponSprite->move(20 * SCALE * dt, 0);
			UpdateBounds();
		}
	}
	if (m_TopBound > Config::GetInstance().GetWindowSizeHeight())
	{
		m_CurrentHealth = 0;
		m_Exploded = false;
	}
}

void Player::Render(sf::RenderWindow * window)
{
	m_Projectiles.front()->Render(window);
	window->draw(*m_pSprite);
	if (m_ShowWeapon)
	{
		window->draw(*m_pWeaponSprite);
		window->draw(*m_pCoverSprite);
	}
	
}
