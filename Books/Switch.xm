#define AUTO_DOWNLOADS
#import "../Common.h"

@interface AutoDownloadBooksSwitch : NSObject <FSSwitchDataSource>
@end

@implementation AutoDownloadBooksSwitch

- (FSSwitchState)stateForSwitchIdentifier:(NSString *)switchIdentifier
{
	BOOL enabled = automaticDownloadsEnabledForCanonicalName(@"AUTO_DOWNLOAD_BOOKS");
	return enabled ? FSSwitchStateOn : FSSwitchStateOff;
}

- (void)applyState:(FSSwitchState)newState forSwitchIdentifier:(NSString *)switchIdentifier
{
	if (newState == FSSwitchStateIndeterminate)
		return;
	setAutomaticDownloadsEnabledForCanonicalName(newState, @"AUTO_DOWNLOAD_BOOKS");
}

@end