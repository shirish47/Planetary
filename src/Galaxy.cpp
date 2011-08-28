//
//  Galaxy.cpp
//  Kepler
//
//  Created by Tom Carden on 6/9/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "Galaxy.h"
#include "Globals.h"

using namespace ci;

void Galaxy::update(const Vec3f &eye, const float &fadeInAlphaToArtist, const float &elapsedSeconds, const Vec3f &bbRight, const Vec3f &bbUp)
{
	// For doing galaxy-axis fades
	mZoomOff = 1.0f - fadeInAlphaToArtist;//constrain( ( G_ARTIST_LEVEL - G_ZOOM ), 0.0f, 1.0f );
	mCamGalaxyAlpha = constrain( abs( eye.y ) * 0.005f, 0.0f, 1.0f );
	mInvAlpha = pow( 1.0f - mCamGalaxyAlpha, 1.75f ) * mZoomOff;    
    mElapsedSeconds = elapsedSeconds;
    mBbRight = bbRight;
    mBbUp = bbUp;
}

void Galaxy::setup(float initialCamDist, ci::Color lightMatterColor, ci::Color centerColor,
ci::gl::Texture galaxyDome, ci::gl::Texture galaxyTex, ci::gl::Texture darkMatterTex, ci::gl::Texture starGlowTex)
{
	mDarkMatterCylinderRes = 48;    
    initGalaxyVertexArray();
    initDarkMatterVertexArray();    
   	mLightMatterBaseRadius = initialCamDist * 0.6075f;
	mDarkMatterBaseRadius = initialCamDist * 0.6375f;

    mLightMatterColor = lightMatterColor;
    mCenterColor = centerColor;
    
    mGalaxyDome = galaxyDome;
    mGalaxyTex = galaxyTex;
    mDarkMatterTex = darkMatterTex;
    mStarGlowTex = starGlowTex;
}

void Galaxy::drawLightMatter( float rotMulti )
{
	gl::enableAdditiveBlending();
	
    // LIGHTMATTER
	if( mInvAlpha > 0.01f ){
		gl::color( ColorA( mLightMatterColor, mInvAlpha ) );
        
		const float radius = mLightMatterBaseRadius;
		glPushMatrix();
        mGalaxyDome.enableAndBind();
		mGalaxyDome.setWrap( GL_REPEAT, GL_REPEAT );
        glBindBuffer(GL_ARRAY_BUFFER, mDarkMatterVBO);
        glVertexPointer( 3, GL_FLOAT, sizeof(VertexData), 0 ); // last arg becomes an offset instead of an address
        glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), (void*)sizeof(Vec3f) ); // NB:- change if type of VertexData.vertex changes
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Leave no VBO bound.                

        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        
		
		float rotationSpeed = -2.0f * mElapsedSeconds;// * rotMulti;
        gl::scale( Vec3f( radius, radius, radius ) );
        gl::rotate( Vec3f( 0.0f, rotationSpeed, 0.0f ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
		
		if( G_IS_IPAD2 ){
			gl::scale( Vec3f( 1.15f, 0.75f, 1.15f ) );
			gl::rotate( Vec3f( 0.0f, 50.0f, 0.0f ) );
			glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
			
			gl::scale( Vec3f( 1.15f, 0.9f, 1.15f ) );
			gl::rotate( Vec3f( 0.0f, 50.0f, 0.0f ) );
			glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
		}
        
        glDisableClientState( GL_VERTEX_ARRAY );
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        mGalaxyDome.disable();
		glPopMatrix();
	}
}

void Galaxy::drawSpiralPlanes( float rotMulti )
{	
    // GALAXY SPIRAL PLANES
	const float alpha = mInvAlpha * mZoomOff;//( 1.25f - mCamGalaxyAlpha ) * mZoomOff;//sqrt(camGalaxyAlpha) * zoomOff;
	if( alpha > 0.01f ){

        gl::color( ColorA( 1.0f, 1.0f, 1.0f, alpha ) );

        glBindBuffer(GL_ARRAY_BUFFER, mGalaxyVBO);
        glVertexPointer( 3, GL_FLOAT, sizeof(VertexData), 0 ); // last arg becomes an offset instead of an address
        glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), (void*)sizeof(Vec3f) ); // NB:- change if type of VertexData.vertex changes
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Leave no VBO bound.                
        
        glPushMatrix();
		mGalaxyTex.enableAndBind();
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		
		if( G_IS_IPAD2 ){
			gl::translate( Vec3f( 0.0f, 3.5f, 0.0f ) );
			gl::rotate( Vec3f( 0.0f, mElapsedSeconds * -4.0f * rotMulti, 0.0f ) );
			glDrawArrays( GL_TRIANGLES, 0, 6 );
			
			gl::translate( Vec3f( 0.0f, -7.0f, 0.0f ) );
			gl::rotate( Vec3f( 0.0f, mElapsedSeconds * -2.0f * rotMulti, 0.0f ) );
			glDrawArrays( GL_TRIANGLES, 0, 6 );
			
			gl::translate( Vec3f( 0.0f, 3.5f, 0.0f ) );
			gl::scale( Vec3f( 0.5f, 0.5f, 0.5f ) );
			gl::rotate( Vec3f( 0.0f, mElapsedSeconds * -15.0f * rotMulti, 0.0f ) );
			glDrawArrays( GL_TRIANGLES, 0, 6 );
		} else {
			gl::rotate( Vec3f( 0.0f, mElapsedSeconds * -4.0f * rotMulti, 0.0f ) );
			glDrawArrays( GL_TRIANGLES, 0, 6 );
		}
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		mGalaxyTex.disable();        
        glPopMatrix();
	}
}

void Galaxy::drawCenter( float rotMulti )
{
    // CENTER OF GALAXY
	const float alpha = mInvAlpha * mZoomOff;//( 1.25f - mCamGalaxyAlpha ) * mZoomOff;
	
	if( alpha > 0.01f ){
		mStarGlowTex.enableAndBind();
		gl::color( ColorA( BRIGHT_BLUE, alpha ) );
		gl::drawBillboard( Vec3f::zero(), Vec2f( 400.0f, 400.0f ), mElapsedSeconds * 10.0f, mBbRight, mBbUp );
		gl::color( ColorA( BRIGHT_YELLOW, alpha ) );
		gl::drawBillboard( Vec3f::zero(), Vec2f( 200.0f, 200.0f ), -mElapsedSeconds * 7.0f, mBbRight, mBbUp );
		mStarGlowTex.disable();
	}
}

void Galaxy::drawDarkMatter( float rotMulti )
{
    // DARKMATTER //////////////////////////////////////////////////////////////////////////////////////////
	if( mInvAlpha > 0.01f ){
		glEnable( GL_CULL_FACE ); 
		glCullFace( GL_FRONT ); 
		
        //float alpha = pow( 1.0f - camGalaxyAlpha, 8.0f ) * zoomOff;
		
		const float radius = mDarkMatterBaseRadius;
        
		glPushMatrix();
		mDarkMatterTex.enableAndBind();
		mDarkMatterTex.setWrap( GL_REPEAT, GL_REPEAT );
        
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );

        glBindBuffer(GL_ARRAY_BUFFER, mDarkMatterVBO);
        glVertexPointer( 3, GL_FLOAT, sizeof(VertexData), 0 ); // last arg becomes an offset instead of an address
        glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), (void*)sizeof(Vec3f) ); // NB:- change if type of VertexData.vertex changes
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Leave no VBO bound.                
        
		gl::color( ColorA( BRIGHT_BLUE, mInvAlpha ) );
		
		float rotationSpeed = mElapsedSeconds * -2.0f;// * rotMulti;
		gl::rotate( Vec3f( 0.0f, rotationSpeed, 0.0f ) );
		gl::scale( Vec3f( radius, radius * 0.75f, radius ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
		
		if( G_IS_IPAD2 ){
			gl::rotate( Vec3f( 0.0f, 50.0f, 0.0f ) );
			gl::scale( Vec3f( 1.15f, 1.15f, 1.15f ) );
			glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
			
			gl::rotate( Vec3f( 0.0f, 50.0f, 0.0f ) );
			gl::scale( Vec3f( 1.15f, 1.15f, 1.15f ) );
			glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
		}
        
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		mDarkMatterTex.disable();
		glPopMatrix();
		glDisable( GL_CULL_FACE ); 
	}
}

void Galaxy::initGalaxyVertexArray()
{
	std::cout << "initializing Galaxy Vertex Array" << std::endl;
    
	VertexData *galaxyVerts = new VertexData[6];

	const float w = 200.0f;

    int vert = 0;

	galaxyVerts[vert].vertex  = Vec3f( -w, 0.0f, -w );
    galaxyVerts[vert].texture = Vec2f::zero();
    vert++;
	
	galaxyVerts[vert].vertex  = Vec3f( w, 0.0f, -w );
    galaxyVerts[vert].texture = Vec2f(1.0f, 0.0f);
    vert++;

    galaxyVerts[vert].vertex  = Vec3f( w, 0.0f, w );
    galaxyVerts[vert].texture = Vec2f(1.0f, 1.0f);
    vert++;

    galaxyVerts[vert].vertex  = Vec3f( -w, 0.0f, -w );
    galaxyVerts[vert].texture = Vec2f(0.0f, 0.0f);
    vert++;

    galaxyVerts[vert].vertex  = Vec3f( w, 0.0f, w );
    galaxyVerts[vert].texture = Vec2f(1.0f, 1.0f);
    vert++;

    galaxyVerts[vert].vertex  = Vec3f( -w, 0.0f, w );
    galaxyVerts[vert].texture = Vec2f(0.0f, 1.0f);
    vert++;
    
    glGenBuffers(1, &mGalaxyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mGalaxyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vert, galaxyVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Leave no VBO bound.        
    
    delete[] galaxyVerts;
}


void Galaxy::initDarkMatterVertexArray()
{
	std::cout << "initializing Dark Matter Vertex Array" << std::endl;
	VertexData *darkMatterVerts = new VertexData[ mDarkMatterCylinderRes * 6 ]; // cylinderRes * two-triangles

    const float TWO_PI = 2.0f * M_PI;
    
	int vert = 0;
	
	for( int x=0; x<mDarkMatterCylinderRes; x++ ){
        
		float per1		= (float)x/(float)mDarkMatterCylinderRes;
		float per2		= (float)(x+1)/(float)mDarkMatterCylinderRes;
		float angle1	= per1 * TWO_PI;
		float angle2	= per2 * TWO_PI;
		
		float sa1 = sin( angle1 );
		float ca1 = cos( angle1 );
		float sa2 = sin( angle2 );
		float ca2 = cos( angle2 );
		
		float h = 0.5f;
		Vec3f v1 = Vec3f( ca1, -h, sa1 );
		Vec3f v2 = Vec3f( ca2, -h, sa2 );
		Vec3f v3 = Vec3f( ca1,  h, sa1 );
		Vec3f v4 = Vec3f( ca2,  h, sa2 );
		
        const float texRepeat = 2.0f;
        
		darkMatterVerts[vert].vertex = v1;
		darkMatterVerts[vert].texture = Vec2f(per1 * texRepeat, 0.0f);
		vert++;

        darkMatterVerts[vert].vertex = v2;
		darkMatterVerts[vert].texture = Vec2f(per2 * texRepeat, 0.0f);
		vert++;

        darkMatterVerts[vert].vertex = v3;
		darkMatterVerts[vert].texture = Vec2f(per1 * texRepeat, 1.0f);
		vert++;

        darkMatterVerts[vert].vertex = v2;
		darkMatterVerts[vert].texture = Vec2f(per2 * texRepeat, 0.0f);
		vert++;

        darkMatterVerts[vert].vertex = v4;
		darkMatterVerts[vert].texture = Vec2f(per2 * texRepeat, 1.0f);
		vert++;

        darkMatterVerts[vert].vertex = v3;
		darkMatterVerts[vert].texture = Vec2f(per1 * texRepeat, 1.0f);
		vert++;
	}
    
    glGenBuffers(1, &mDarkMatterVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mDarkMatterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vert, darkMatterVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Leave no VBO bound.        
    
    delete[] darkMatterVerts;
}
