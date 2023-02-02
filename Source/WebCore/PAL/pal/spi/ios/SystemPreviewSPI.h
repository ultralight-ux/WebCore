/*
 * Copyright (C) 2018 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#if USE(APPLE_INTERNAL_SDK)

#if HAVE(ARKIT_QUICK_LOOK_PREVIEW_ITEM)
#import <AssetViewer/ARQuickLookWebKitItem.h>
#endif

#import <AssetViewer/ASVThumbnailView.h>

#else

#import <UIKit/UIKit.h>

@class ASVThumbnailView;
@class QLItem;
@class QLPreviewController;

@protocol ASVThumbnailViewDelegate <NSObject>
- (void)thumbnailView:(ASVThumbnailView *)thumbnailView wantsToPresentPreviewController:(QLPreviewController *)previewController forItem:(QLItem *)item;
@end

@interface ASVThumbnailView : UIView
@property (nonatomic, weak) id<ASVThumbnailViewDelegate> delegate;
@property (nonatomic, assign) QLItem *thumbnailItem;
@property (nonatomic) CGSize maxThumbnailSize;
@end

#if HAVE(ARKIT_QUICK_LOOK_PREVIEW_ITEM)
#import <ARKit/ARKit.h>

@protocol ARQuickLookWebKitItemDelegate
@end

@class ARQuickLookWebKitItem;

@interface ARQuickLookWebKitItem : QLItem
- (instancetype)initWithPreviewItemProvider:(NSItemProvider *)itemProvider contentType:(NSString *)contentType previewTitle:(NSString *)previewTitle fileSize:(NSNumber *)fileSize previewItem:(ARQuickLookPreviewItem *)previewItem;
- (void)setDelegate:(id <ARQuickLookWebKitItemDelegate>)delegate;
@end

#endif

#endif
