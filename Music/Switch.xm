#define AUTO_DOWNLOADS
#import "../Common.h"

@interface AutoDownloadMusicSwitch : NSObject <FSSwitchDataSource>
@end

@implementation AutoDownloadMusicSwitch

- (FSSwitchState)stateForSwitchIdentifier:(NSString *)switchIdentifier
{
	BOOL enabled = automaticDownloadsEnabledForCanonicalName(@"AUTO_DOWNLOAD_MUSIC");
	return enabled ? FSSwitchStateOn : FSSwitchStateOff;
}

- (void)applyState:(FSSwitchState)newState forSwitchIdentifier:(NSString *)switchIdentifier
{
	if (newState == FSSwitchStateIndeterminate)
		return;
	setAutomaticDownloadsEnabledForCanonicalName(newState, @"AUTO_DOWNLOAD_MUSIC");
}

@end