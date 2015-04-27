#pragma once
#include "sssf_VS\stdafx.h"

class PhysicalProperties
{
protected:
	float buoyancy;
	float mass;
	bool  collidable;
	bool  fireStraight;
	bool  dead;
	float coefficientOfRestitution;
	float x;
	float y;
	float z;
	float velocityX;
	float velocityY;
	float accelerationX;
	float accelerationY;
	int spriteType;
	int deathCount;
	int shotTimer;
	bool isInvincible;

public:
	PhysicalProperties()	{ accelerationX = 0; accelerationY = 0; fireStraight = true; dead = false; deathCount = 0; shotTimer = 0; isInvincible = false; }
	~PhysicalProperties()	{}

	// INLINED METHODS
	float getBuoyancy()						{ return buoyancy;				}
	float getMass()							{ return mass;					}
	float getCoefficientOfRestitution()		{ return coefficientOfRestitution;	}
	bool isCollidable()						{ return collidable;			}
	float getX()							{ return x;						}
	float getY()							{ return y;						}
	float getZ()							{ return z;						}
	float getVelocityX()					{ return velocityX;				}
	float getVelocityY()					{ return velocityY;				}
	float getAccelerationX()				{ return accelerationX;			}
	float getAccelerationY()				{ return accelerationY;			}
	bool getFireDir()						{ return fireStraight;			}
	bool getDead()							{ return dead;					}
	bool getInvincible()					{ return isInvincible;			}
	void setInvincible(bool initInv)		{ isInvincible = initInv;		}	
	int  getShot()							{ return shotTimer;				}
	void setShot(int initShot)				{ shotTimer = initShot;			}
	int  getSpriteType()					{ return spriteType;			}
	void setSpriteType(int initSpriteType)	{ spriteType = initSpriteType;  }
	int  getDeathCount()					{ return deathCount;			}
	void setDeathCount(int initDeath)	    { deathCount = initDeath;		}
	void swapFireDir()						{ fireStraight = !fireStraight; }
	void setDead()							{ dead = true; 				    }
	void setBuoyancy(float initBuoyancy)	{ buoyancy = initBuoyancy;		}
	void setMass(float initMass)			{ mass = initMass;				}
	void setCofficientOfRestitution(float cR) { coefficientOfRestitution = cR; }
	void setCollidable(bool initCollidable)	{ collidable = initCollidable;	}
	void setX(float initX)					{ x = initX;					}
	void incX(float xToInc)					{ x += xToInc;					}
	void setY(float initY)					{ y = initY;					}
	void incY(float yToInc)					{ y += yToInc;					}
	void setZ(float initZ)					{ z = initZ;					}

	int round(float realNum)
	{
		return (int)floor(realNum + 0.5f);
	}

	void setPosition(float initX, float initY)
	{
		x = initX;
		y = initY;
	}

	void setVelocity(float initVx, float initVy)
	{
		velocityX = initVx;
		velocityY = initVy;
	}

	void incVelocity(float incVx, float incVy)
	{ 
		velocityX += incVx;
		velocityY += incVy;
	}
	void setAccelerationX(float initAx)		{ accelerationX = initAx;		}
	void setAccelerationY(float initAy)		{ accelerationY = initAy;		}

	float calcTotalVelocity()
	{
		float xSquared = x * x;
		float ySquared = y * y;
		float totalVelocity = xSquared + ySquared;
		totalVelocity = sqrtf(totalVelocity);
		return totalVelocity;
	}

	void applyAcceleration()
	{
		velocityX += accelerationX;
		velocityY += accelerationY;
	}
};